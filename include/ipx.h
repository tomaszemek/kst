// project: Communication protocol, v. 1.0
// module: ipx.h
// description: C_IPX class description
// (c) Tomas Zemek, 2003, 2025

#ifndef _IPX_H
#define _IPX_H

// project headers:
	#include "types.h"
	#include "globvars.h"
	#include "frame.h"
	#include "constant.h"

// return values of IPX functions:
class C_IPX_Status {

  public:

	enum ipxReturnCode {

	  IpxSuccess = 0,
	  IpxSocketTableIsFull,
	  IpxSocketAlreadyOpen,
	  IpxSocketNotOpen,
	  IpxNoPathToDestination,
	  IpxCannotCancelEcb,
	  IpxEcbIsInUse,
	  IpxEcbIsNotInUse,
	  IpxDataTooLong,
	  IpxAlreadyInUse,
	  IpxError

	};

	enum ecbCompletionCode {

	  EcbOk = 0,
	  EcbEventCanceled = 0xFC,
	  EcbPacketMalformed = 0xFD,
	  EcbUndeliverablePacket = 0xFE,
	  EcbNetworkFailure = 0xFF,
	  EcbBufferTooSmall = 0xFD,
	  EcbSocketNotOpen = 0xFF,
	  EcbCannotCancel = 0xF9

	};

	enum ecbInUse {

	  EcbInUseFree = 0x00,
	  EcbInUseProcessing = 0xFA,
	  EcbInUseWaiting = 0xFB,
	  EcbInUseWaitingForAESTimeout = 0xFC,
	  EcbInUseWaitingForTimeout = 0xFD,
	  EcbInUseListening = 0xFE,
	  EcbInUseSending = 0xFF

	};

};

// types: network address, node address:
	typedef byte T_Net_Address[4];
	typedef byte T_Node_Address[6];

// type: ESR function
	typedef void (huge *FP_ESR)(void);	//	service routine

// IPX data types:
	// IPX packet header:
	// '!': mandatory attributes
	struct S_IPX_Packet_Head {

		word			m_checksum;		 	// 	checksum, HLword
		word			m_length;			// 	length, HLword
		byte			m_ttl;				// 	transport control
		byte			m_packet_type;		//	! packet type
		T_Net_Address	m_dest_net;			//	! destination network
		T_Node_Address 	m_dest_node;		//	! destination node
		word			m_dest_socket;		//	! destination socket, HLword
		T_Net_Address	m_src_net;			// 	source net
		T_Node_Address	m_src_node;			// 	source node
		word			m_src_socket;		// 	source socket, HLword

	};

	// IPX packet:
	struct S_IPX_Packet {

		S_IPX_Packet_Head	m_head;			//	packet header
		byte				m_data[_IPX_Packet_Data_Size];   //	packet data

	};

	typedef S_IPX_Packet volatile huge * FP_IPX_Packet;

	// Event Control Block:
	// 'r': mandatory for receive, 's': mandatory for send
	struct S_ECB {

		byte			m_ECB_link_address[4];	//   	link address
		FP_ESR			m_ESR_address;			// rs	service routine
		byte			m_in_use;				//		'in use' flag
		byte			m_completition_code;	//		completion code
		word			m_socket;				// rs	socket number, HLword
		byte			m_workspace[16];		//		work space for IPX and driver
		T_Node_Address	m_next_hop;   			// s	next hop node address
		word			m_fragment_count;		// rs	fragment count (1)
		FP_IPX_Packet	m_fragment_fp;			// rs	fragment address (1)
		word			m_fragment_size;		// rs	fragment size (1)
	};

	typedef S_ECB huge * FP_ECB;

// IPX driver encapsulation:
class C_IPX {

	// sending administration:
		S_IPX_Packet volatile huge *m_send_packet;
		S_ECB volatile huge *m_send_ecb;

	// receiving administration:
		// solved by local variables in module ipx.cpp

		// helper variable for receive:
		// received data are place in a huge* buffer. The rest of the program
		// works either with far* or near* data, that's why we perform
		// copy to here, and only this buffer is given to method C_Frame::Set()
		byte *m_help_buffer;

	// I/O operation error code:
		uint m_ipx_error;

	// private methods:
	// initialization:
		bool IPX_Init();

	// socket operations:
		bool Open_Socket();
		void Close_Socket();

	// network operations:
		void Send();

		// method Listen() has been moved to local level of the module ipx.cpp
		
	// stop with receiving for packets:
		void Cancel_Recv_Event();

	public:

	// constructor, destructor:
		C_IPX();
		~C_IPX();

	// initialization:
		// flush the queue, reset:
		void Init();

	// explicit traffic control
		// flush the buffers, starts with receiving/sending packets
		void Start();

		// flush the buffers, stops with receiving/sending packets
		void Stop();

	// explicit send or get a frame operation:
		void Send(C_Frame *frame);	// put frame on the network
									// it waits until sent

		void Get(C_Frame *frame);	// pull first element from the receive queue

	// test for data presence in receive queue 
		// returns true if the first waiting frame has header == parameter header,
		// or if there is any frame waiting at all 
		// (as stated by the implicit parameter)
		bool Ready(const byte header = 0);

	// dump buffer indexes to the log file
		void Dump();

};

#endif