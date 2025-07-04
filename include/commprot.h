// project: Communication protocol, v. 1.0
// module: commprot.h
// description: C_Commprotocol class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Commprotocol_H
#define _Commprotocol_H

// projektove hlavicky:
	#include "event.h"
	#include "kbd.h"
	#include "frame.h"
	#include "fcs.h"
	#include "timer.h"
	#include "sender.h"
	#include "receiver.h"
	#include "ipx.h"
	#include "keybuf.h"
	#include "globvars.h"
	#include "constant.h"

// class 'Communication protocol':
// created as per requirements:
// - half duplex communication channel between computers of type PC
// - link layer
// - use protocol IPX for actual transmission
// - use frames with such structure:
//		- header (ENQ, ACK, DAT, NAK, EOT) - 1 byte
//		- number of sent frame - 1 byte
//		- number of expected frame - 1 byte
//		- sender address - 1 byte
//		- receiver address - 1 byte
//		- length of data; contains 0 for control frames - 1 byte
//		- carried data (not set if length of data is zero) - max 255 bytes
//		- FCS - checksum (only for DAT frames)
// - expected functionality:
//		- simple chat, frames used to minimum, field 'data' contains always 
//			just a single character;
//			frame numbering and acknowledging (positive - ACK, negative - NAK)
//			are used to check delivery in correct sequence
//
// chosen implementation approach: finite state machine - known set of possible protocol 
//		states and state transitions defined upfront.
class C_Commprotocol {

	// members:
		// keyboard input buffer:
		C_KeyBuffer *m_keybuffer;

		// current frame:
		C_Frame *m_send_frame;	// frame being sent
		C_Frame *m_recv_frame;	// frame being received

		// checksum for DAT frames:
		C_FCS *m_fcs;

		// timers:
		C_Timer *m_timer_A;		// to control connection establishment
		C_Timer *m_timer_B;		// to keep connection established
		C_Timer *m_timer_C;		// to determine connection disconnected
		C_Timer *m_timer_D;		// to control connection disconnect

		// sending/receiving frame windows:
		C_Sender *m_sender;		// sender / sending window, sends DAT frames out and processes received acknowledgements for the sent frames
		C_Receiver *m_receiver;	// receiver / receiving window, receives DAT frames, checks sequence and sends acknowledgements for the received frames

		// low level network layer
		C_IPX *m_ipx;	// actual network (from perspective of the communication protocol is IPX lower layer)

		// buffer for the purpose of the frame monitor functionality
		// acts as an intermediate step before display on screen
		byte *m_frame_monitor_buffer;

		// counter for connection setup functionality:
		uint m_connecting_counter;

		// index to the array of priority indicators:
		uint m_priority_index;

		// current state of the protocol (of the state machine)
		uint m_state;

		// state switcher
		// TODO: define array of strings indexed by state value and log those strings too
		void Change_State(uint new_status);

		// indication of connection disconnected and state machine finished
		bool m_finished;

		// int connection setup indicator
		int m_count_ENQ;

	public:

		C_Commprotocol();
		~C_Commprotocol();

		// the only possible event that could be interested for class users
		// is the fact whether connection has been disconnected and state machine 
		// has finished
		inline bool Is_Event() { return m_finished;}

		inline void Get_Event(C_Event *event)
		{
			event->msg.code = msg_sm_finished;
			m_finished = false;
		}

		// protocol main loop
		void Process_Event(C_Event *event);

};


#endif // _Commprotocol_H