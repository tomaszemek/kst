// project: Communication protocol, v. 1.0
// module: ipx.cpp
// description: C_IPX class description
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <dos.h>
	#include <mem.h>

// project headers:
	#include "ipx.h"

// external variables:
	// flag indicating whether monitor mode is active
	import bool g_frame_monitor_mode;

// local variables:
	// addressing:
	local T_Node_Address l_broadcast = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	local T_Net_Address l_local_net = { 0, 0, 0, 0};

	// receiving administration:
	local S_IPX_Packet volatile huge *l_recv_buffer[_Recv_Buffer_Size];
											// incoming data are put here by ECB
											// then it triggers ESR which copies them
											// at the right place in recv_buffer
	local S_ECB volatile huge *l_recv_ecb;	// defines where to store incoming data

	// indexes:
	local byte volatile l_recv_write;
	local byte volatile l_recv_read;
	local byte volatile l_recv_empty;
	local byte volatile l_recv_full;

	// counters:
	local ulong volatile l_packet_counter;
	local ulong volatile l_lost_packets;

	// other variables:
	// bool volatile l_received;
	local uint volatile l_ipx_error;

	// helper counter:
	local ulong volatile l_temp_counter;

// C_IPX class methods:
// private methods:
uint HiLo(uint lohi) {

   uint ret = ((lohi % 256) * 256);

   return (ret + (lohi / 256));
}

bool C_IPX::IPX_Init() {

   struct REGPACK r;
   bool retval;

   r.r_ax = 0x7a00;

   intr(0x2F, &r);

   if((m_ipx_error = r.r_ax % 256) != 0xff) retval = false;
   else retval = true;

   #ifdef _Log_Type_Trace
   g_log->Log("\n C_IPX::IPX_Init() - operation result = '%d'",m_ipx_error);
   #endif

   return retval;
}

bool C_IPX::Open_Socket() {

   struct REGPACK r;
   bool retval;

   r.r_bx = 0;	// service 'open socket'
   r.r_ax = 0;	// socket remains open until closed or until program terminated

   r.r_dx = _KST_Socket;	// socket number

   intr(_IPX_Interrupt_Number, &r);

   switch(r.r_ax % 256) {	// = value from AL

		case 0:

			m_ipx_error = C_IPX_Status::IpxSuccess;
			break;

		case 0xFE:

			m_ipx_error = C_IPX_Status::IpxSocketTableIsFull;
			break;

		case 0xFF:

			m_ipx_error = C_IPX_Status::IpxSocketAlreadyOpen;
			break;

		default:

			m_ipx_error = C_IPX_Status::IpxError;
			break;
   }

   if(r.r_dx != _KST_Socket) retval = false;
   else {

		if(m_ipx_error != C_IPX_Status::IpxSuccess) retval = false;
		else retval = true;
   }

   return retval;
}

void C_IPX::Close_Socket() {

	struct REGPACK r;

	r.r_bx = 1;				// service 'close socket'
	r.r_dx = _KST_Socket;	// socket number

	intr(_IPX_Interrupt_Number, &r);

	return;
}

void C_IPX::Cancel_Recv_Event() {

	struct REGPACK r;

	r.r_bx = 6;
	r.r_es = FP_SEG(l_recv_ecb);
	r.r_si = FP_OFF(l_recv_ecb);

	intr(_IPX_Interrupt_Number, &r);

	switch(r.r_ax % 256) {

		case 0:

			m_ipx_error = C_IPX_Status::IpxSuccess;
			break;

		case 0xF9:

			// ECB in progress being processed
			m_ipx_error = C_IPX_Status::IpxCannotCancelEcb;
			break;

		case 0xFC:

			// ECB has been cancelled
			m_ipx_error = C_IPX_Status::EcbEventCanceled;
			break;

		case 0xFF:

			// ECB is not in use, OK
			m_ipx_error = C_IPX_Status::IpxEcbIsNotInUse;
			break;

		default:

			// other error:
			m_ipx_error = C_IPX_Status::IpxError;
			break;

	}

	return;
}


// conditions for correct behavior:
// before sending, ECB must be correctly set
void C_IPX::Send() {

	struct REGPACK r;

	r.r_bx = 3;
	r.r_es = FP_SEG((void far *) m_send_ecb);
	r.r_si = FP_OFF((void far *) m_send_ecb);

	intr(_IPX_Interrupt_Number, &r);

	while(m_send_ecb->m_in_use != C_IPX_Status::EcbInUseFree)	;	// wait until free

	return;

}

void Listen() {

	struct REGPACK r;

	r.r_bx = 4;
	r.r_es = FP_SEG(l_recv_ecb);
	r.r_si = FP_OFF(l_recv_ecb);

	intr(_IPX_Interrupt_Number, &r);

	switch(r.r_ax % 256) {

		case 0:

			l_ipx_error = C_IPX_Status::IpxSuccess;
			break;

		case 0xFF:

			l_ipx_error = C_IPX_Status::IpxSocketNotOpen;

			l_temp_counter++;

			break;

		default:

			l_ipx_error = C_IPX_Status::IpxError;

			l_temp_counter++;

			break;
	}

	return;

}


// receiving ESR:
// REQUIRED _loadds !!! (won't work otherwise without)
void far _loadds _saveregs recv_ESR(void) {

	// - ESR is triggered once action completed:
	// - if we update ECB and start Listen again, it should in theory keep working
	// - here in the ESR we focus only on follow up work needed to be done once packet was received
	// - we won't call any other function if possible

	// counter of received packets
	l_packet_counter++;

	// if in chat mode, test if packet is for us:
	if(g_frame_monitor_mode == false) {

		// is it for us?
		if(l_recv_buffer[l_recv_write]->m_data[4] != g_local_id) {

			// it's not for us, meaning the writing index can remain pointing the same place,
			// as we don't care about what has been received

			// set the ECB ready for receiving new packets
			// mark ECB as in_use:
			l_recv_ecb->m_in_use = 0xFF;

			// point storage buffer of the ECB to the next array item:
			l_recv_ecb->m_fragment_fp = l_recv_buffer[l_recv_write];
			l_recv_ecb->m_fragment_size = (_IPX_Packet_Header_Size + _IPX_Packet_Data_Size);

			// other ECB attributes remain unmodified

			// enable receiving to network activity again:
			Listen();

			return;
		}
	}

	// if it's full, drop it
	if(l_recv_full == true) {

		l_lost_packets++;

		// set the ECB ready for receiving new packets
		// mark ECB as in_use:
		l_recv_ecb->m_in_use = 0xFF;

		// point storage buffer of the ECB to the next array item:
		l_recv_ecb->m_fragment_fp = l_recv_buffer[l_recv_write];
		l_recv_ecb->m_fragment_size = (_IPX_Packet_Header_Size + _IPX_Packet_Data_Size);

		// other ECB attributes remain unmodified

		// enable receiving to network activity again:
		Listen();

		return;
	}

	// update writing index:
	if((++l_recv_write) == _Recv_Buffer_Size) l_recv_write = 0;

	// set bool flags and stop listening eventually
	// test if buffer is full:
	if(l_recv_write == l_recv_read) {

		l_recv_full = true;

		return;
	}

	// if it was full, it's not anymore (ESR is called after data were stored)
	if(l_recv_empty == true) l_recv_empty = false;

	// set the ECB ready for receiving new packets
	// mark ECB as in_use:
	l_recv_ecb->m_in_use = 0xFF;

	// point storage buffer of the ECB to the next array item:
	l_recv_ecb->m_fragment_fp = l_recv_buffer[l_recv_write];
	l_recv_ecb->m_fragment_size = (_IPX_Packet_Header_Size + _IPX_Packet_Data_Size);

	// other ECB attributes remain unmodified

	// enable receiving to network activity again:
	Listen();

	return;

}

// public methods:
C_IPX::C_IPX() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::C_IPX() - instance constructon started : ");
	g_log->Log("\n C_IPX::C_IPX() - test: size of 'unsigned short' type = %d B",sizeof(unsigned short));
	#endif

	// initialize:
	m_ipx_error = 0;
	l_ipx_error = 0;

	// initialize driver:
	if(IPX_Init() == false)

		g_error->Error("C_IPX::C_IPX() - IPX services not available ");

	else {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_IPX::C_IPX() - IPX services are available");
		#endif
	}

	// open socket:
	// try closing it first and opening later, as it might have remained opened,
	// due to previous program abnormal termination
	if(Open_Socket() == false) {

		g_error->Error("C_IPX::C_IPX() - can't open socket %u", _KST_Socket);

	}

	// construct sending administration objects:
	m_send_packet = (S_IPX_Packet volatile huge *) new S_IPX_Packet;

	if(m_send_packet == NULL)

		g_error->Error("C_IPX::C_IPX() - 'm_send_packet': error during memory allocation");

	m_send_ecb = (S_ECB volatile huge *) new S_ECB;

	if(m_send_ecb == NULL)

		g_error->Error("C_IPX::C_IPX() - 'm_send_ecb': error during memory allocation");

	// construct receiving administration objects:
	for(int i=0;i<_Recv_Buffer_Size;i++) {

		l_recv_buffer[i] = (S_IPX_Packet volatile huge *) new S_IPX_Packet;

		if(l_recv_buffer[i] == NULL)

			g_error->Error("C_IPX::C_IPX() - 'l_recv_packet[i]': error during memory allocation");

	}

	l_recv_ecb = (S_ECB volatile huge *) new S_ECB;

	if(l_recv_ecb == NULL)

		g_error->Error("C_IPX::C_IPX() - 'l_recv_ecb': error during memory allocation");

	// help buffer:
	m_help_buffer = new byte[_IPX_Packet_Data_Size];

	if(m_help_buffer == NULL)

		g_error->Error("C_IPX::C_IPX() - 'l_help_buffer': error during memory allocation");

	// initialize variables, including IPX objects:
	Init();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::C_IPX() - instance construction completed");
	#endif

	return;
}

C_IPX::~C_IPX() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::~C_IPX() - instance destruction started ; ");
	#endif

	// count of received but unprocessed packets:
	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::~C_IPX() - count of received packets = %lu",l_packet_counter);
	#endif

	Cancel_Recv_Event();

	if ((m_ipx_error != C_IPX_Status::IpxSuccess) && (m_ipx_error != C_IPX_Status::IpxEcbIsNotInUse))

		#ifdef _Log_Type_Error
		g_log->Log("\n C_IPX::~C_IPX() - failed to destroy receiving ECB, m_ipx_error = %u",m_ipx_error);
		#else
		;
		#endif

	// close socket:
	Close_Socket();

	// destroy local variables of this module:
	// sending set:
	delete (void far *)m_send_ecb;
	delete (void far *)m_send_packet;

	// receiving set:
	delete (void far *)l_recv_ecb;

	for(int i=0;i<_Recv_Buffer_Size;i++) delete (void far *)l_recv_buffer[i];

	delete [] m_help_buffer;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::~C_IPX() - instance destruction completed");
	#endif

	return;
}

void C_IPX::Init() {

	// helper variable:
	int i;

	// sending administration:
	// IPX packet:
	_fmemset((void far *)m_send_packet,'\0',(_IPX_Packet_Header_Size + _IPX_Packet_Data_Size));

	m_send_packet->m_head.m_packet_type = 0;	// = unknown type
	for(i=0;i<4;i++) m_send_packet->m_head.m_dest_net[i] = l_local_net[i];
	for(i=0;i<6;i++) m_send_packet->m_head.m_dest_node[i] = l_broadcast[i];
	m_send_packet->m_head.m_dest_socket = HiLo(_KST_Socket);	// destination socket

	// ECB:
	_fmemset((void far *)m_send_ecb,'\0',(_IPX_ECB_Length));

	m_send_ecb->m_socket = HiLo(_KST_Socket);
	for(i=0;i<6;i++) m_send_ecb->m_next_hop[i] = l_broadcast[i];
	m_send_ecb->m_fragment_count = 1;
	m_send_ecb->m_fragment_fp = m_send_packet;
	m_send_ecb->m_fragment_size = (_IPX_Packet_Header_Size + _IPX_Packet_Data_Size);

	// receiving administration:
	// IPX packet:
	for(i=0;i<_Recv_Buffer_Size;i++)

		_fmemset((void far *)l_recv_buffer[i],'\0',(_IPX_Packet_Header_Size + _IPX_Packet_Data_Size));

	// ECB:
	_fmemset((void far *)l_recv_ecb,'\0',(_IPX_ECB_Length));

	l_recv_ecb->m_ESR_address = recv_ESR;
	l_recv_ecb->m_in_use = 0xFF;
	l_recv_ecb->m_socket = HiLo(_KST_Socket);
	for(i=0;i<6;i++) l_recv_ecb->m_next_hop[i] = l_broadcast[i];
	l_recv_ecb->m_fragment_count = 1;
	l_recv_ecb->m_fragment_fp = l_recv_buffer[0];
	l_recv_ecb->m_fragment_size = (_IPX_Packet_Header_Size + _IPX_Packet_Data_Size);

	// helper variables:
	l_packet_counter = 0;
	l_lost_packets = 0;

	l_ipx_error = 0;

	l_temp_counter = 0;

	// indexes:
	l_recv_write = 0;
	l_recv_read = 0;
	l_recv_empty = true;
	l_recv_full = false;

	return;
}

void C_IPX::Start() {

	// initialize variables:
	Init();

	Listen();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::C_IPX() - method listen() called, result = %u",l_ipx_error);
	#endif

	return;
}

void C_IPX::Stop() {

	Cancel_Recv_Event();

	if ((m_ipx_error != C_IPX_Status::IpxSuccess) && (m_ipx_error != C_IPX_Status::IpxEcbIsNotInUse))

	#ifdef _Log_Type_Error
	g_log->Log("\n C_IPX::Stop() - failed to destroy receiving ECB, m_ipx_error = %u",m_ipx_error);
	#else
	;
	#endif

	// dump session statistics:
	#ifdef _Log_Type_Dump
	g_log->Log("\n C_IPX::Stop() - final statistics: - dump:");

	Dump();
	#endif

	return;
}

void C_IPX::Send(C_Frame *frame) {

	// fill data part of the ipx packet with zeros:
	_fmemset((void far *)(m_send_packet->m_data),'\0',(_IPX_Packet_Data_Size));

	// copy 'buffer' from the sending frame to the ipx packet:
	_fmemcpy((void far *)(m_send_packet->m_data), (const void far *)frame->Get_Frame_Buffer(), frame->Get_Frame_Buffer_Size());

	// check what's being sent:
	#ifdef _Log_Type_Dump
	g_log->Log("\n C_IPX::Send() - sending packet: \n");

	for(int i=0;i<(_KST_Packet_Data_Size_Modified);i++)

		g_log->Log("%u,",m_send_packet->m_data[i]);

	#endif

	Send();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_IPX::Send() - completed, m_ipx_error = %u", m_ipx_error);
	#endif

	return;
}

void C_IPX::Get(C_Frame *frame) {

	// copy data to near buffer:
		// this has been modified agains specification; original condition was:
		// i<IPX_Packet_Data_Length
		// the reason being optimization; only part of whole data is being used by the chat app
	for(int i=0;i<_KST_Packet_Data_Size_Modified;i++) m_help_buffer[i] = l_recv_buffer[l_recv_read]->m_data[i];

	// read it
	frame->Set(m_help_buffer);

	// update indexes:
	if((++l_recv_read) == _Recv_Buffer_Size) l_recv_read = 0;

	// booleans:
	// is it empty now?
	if(l_recv_read == l_recv_write) l_recv_empty = true;

	// was it full before calling the current method?
	if(l_recv_full == true) {

		l_recv_full = false;

		// set the ECB ready for receiving new packets
		// mark ECB as in_use:
		l_recv_ecb->m_in_use = 0xFF;

		// point storage buffer of the ECB to the next array item:
		l_recv_ecb->m_fragment_fp = l_recv_buffer[l_recv_write];

		// other ECB attributes remain unmodified

		// enable receiving to network activity again:
		Listen();
	}

	return;
}

bool C_IPX::Ready(const byte header) {

	bool retval;

	// test for empty receiving queue:
	if(l_recv_empty == true) retval = false;
	else {

		// test header in the queue head item, compare with parameter
		if(l_recv_buffer[l_recv_read]->m_data[0] == header)

			retval = true;

		else {

			// if it was requested to test presence of specific frame type and the test failed, 
			// then false needs to be returned 
			if(header != 0) retval = false;	// the current head frame type doesn't match provided parameter

			// but if was requested to test presence of any frame type (parameter being set to 0 or omitted),
			// then true needs to be returned 
			else retval = true;		// yes there is a frame waiting

		}

	}

	return retval;

}

void C_IPX::Dump() {

	#ifdef _Log_Type_Dump
	// local variables:
	int i,j;

	g_log->Log("\n ***** C_IPX::Dump - dump local variables state");

	g_log->Log("\n -- outgoing packet: ");

	for(i=0;i<_KST_Packet_Data_Size_Modified;i++) g_log->Log("%u,",m_send_packet->m_data[i]);

	g_log->Log("\n -- incomign buffer:");

	g_log->Log("\n l_recv_write = %d, l_recv_read = %d, l_recv_full = %d, l_recv_empty = %d",l_recv_write,l_recv_read,l_recv_full,l_recv_empty);

	for(j=0;j<_Recv_Buffer_Size;j++) {

		g_log->Log("\n -- l_recv_buffer[%d] : -- ",j);

		for(i=0;i<_KST_Packet_Data_Size_Modified;i++)

			g_log->Log("%u,",l_recv_buffer[j]->m_data[i]);

	}

	g_log->Log("\n There were in total %lu packets processed, %lu packets got lost, l_temp_counter == %lu",l_packet_counter, l_lost_packets, l_temp_counter);
	#endif

	return;
}