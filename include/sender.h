// project: Communication protocol, v. 1.0
// module: sender.h
// description: C_Sender class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Sender_H
#define _Sender_H

// project headers:
	#include "types.h"
	#include "globvars.h"
	#include "frame.h"
	#include "constant.h"

// C_Sender represents the sender:
// - encapsulates the sending window, receives characters and puts them
//  into the window, provides complete DAT frame to send and receives
//  acknowledging frames from the remote peer sender
class C_Sender {

	// data structure of the window
		// static memory allocation, 16 items (for KST we work with window size 16)
		// if running out of memory, need to rewrite for dynamic memory allocation
		C_Frame **m_window;

	// window indexes:
		uint m_index_keys;	// position in the window where data from keyboard should be stored at
		uint m_index_send;	// position of the frame that should be sent
		uint m_index_ackd;	// position of the frame for which we await acknowledgement to be received

	// window state:
		bool m_state_empty;
		bool m_state_full;

	public:

		C_Sender();
		~C_Sender();

		// reset the contents (make it empty)
		void Init();

	// work with the window:
		// add character (key code) to the window:
		// returns -1 on failure (window full)
		// returns 0 on success
		int Add_Char(uint key);

		// read frame at the queue head
		// TODO: the target frame mustn't be destroyed earlier than it got processed
		// 		 how to achieve that?
		// returns -1 on failure (window empty)
		// returns 0 on success
		int Get_Frame_Buffer(C_Frame *frame);

		// update window based on received frame:
		void Update(C_Frame *frame);

	// window state:
		inline bool Is_Empty() { return m_state_empty;}
		inline bool Is_Full() { return m_state_full;}

};


#endif