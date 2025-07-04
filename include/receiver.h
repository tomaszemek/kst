// project: Communication protocol, v. 1.0
// module: receiver.h
// description: C_Receiver class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Receiver_H
#define _Receiver_H

// project headers:
	#include "types.h"
	#include "frame.h"
	#include "globvars.h"
	#include "constant.h"

// class C_Receiver represents receiver
// receives frames of type DAT, verifies that they arrived in order
// and tracks evidence about sent acknowledgements of the sender 
// of the remote peer station
class C_Receiver {

	// data members
		// acknowledging window
		byte m_reply_buffer[_SendingFrameWindow_Size];

		// indexes:
		byte m_index_expected;	// expected number of the frame to receive
								// keeps track of the order

		byte m_index_send;	// points to frame to send

		// values from frame:
		byte m_recv_Ns;	// value Ns from the received DAT frame
		byte m_recv_Nr;	// value Nr from the received DAT frame

	// state:
		bool m_bool; 	// did it run out of sync,
						// i.e. is it needed to send something
						// out of regular order?

	public:

		C_Receiver();
		~C_Receiver();

		void Init();

		// verification:
		// performs test and prepares result
		bool Verify(C_Frame *frame);

		// what shall be sent as acknowledgement:
		// frame will be constructed by Verify()
		void Get_Reply(C_Frame *frame);

		// returns state of the acknowledging window
		// have all acknowledgements been sent?
		// returns true if anything still left to send
		// returns false if nothing left to send
		inline bool Need_Send() { return m_bool;}

		// apparently not implemented yet:
		// TODO
		inline bool Is_Full() { return false;}

};

#endif


