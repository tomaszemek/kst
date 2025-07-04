// project: Communication protocol, v. 1.0
// module: sender.cpp
// description: C_Sender class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	// none used

// project headers:
	#include "sender.h"

// C_Sender methods:
C_Sender::C_Sender() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Sender::C_Sender() - instance construction started : ");
	#endif

	// construct the window buffer (array of frames):
	m_window = new C_Frame *[_SendingFrameWindow_Size];

		if(m_window == NULL) g_error->Error("C_Sender::C_Sender() - 'm_window': failed to allocate memory");

	// now allocate each frame:
		for(int i = 0;i<_SendingFrameWindow_Size;i++) {

		m_window[i] = new C_Frame;

		if(m_window[i] == NULL) g_error->Error("C_Sender::C_Sender() - 'm_window[%d]': failed to allocate memory", i);
	}

	// reset (empty) the window
	Init();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Sender::C_Sender() - instance construction completed");
	#endif

	return;
}

C_Sender::~C_Sender() {

	// destruct the window
	// TODO

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Sender::~C_Sender() - instance destruction complete");
	#endif

	return;
}

void C_Sender::Init() {

	// reset each frame:
	for(int i=0;i<_SendingFrameWindow_Size;i++) {

		m_window[i]->Reset();
	}

	// set window empty
	m_state_empty = true;
	m_state_full = false;

	m_index_keys = 0;
	m_index_send = 0;
	m_index_ackd = 0;

	return;
}

int C_Sender::Add_Char(uint key) {

	// test for window full:
	if(m_state_full == true) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Sender::Add_Char() - window is full");
		#endif

		return (-1);
	}

	// store the key and set frame type to DAT:
	m_window[m_index_keys]->Set_DAT(key);

	// set other frame attributes:
	//   the numbers are set just before sending  - when reading from the window with Get_Frame_Buffer()

	// update indexes:
	// keys:
	if((++m_index_keys) == _SendingFrameWindow_Size) m_index_keys = 0;

	// is window full now? the m_state_full can be only reset in Update() once the respective Ack was received from the remote peer station
	if((m_index_keys == (m_index_ackd - 1)) || ((m_index_keys == _SendingFrameWindow_Size - 1) && (m_index_ackd == 0))) m_state_full = true;

	// window not empty anymore:
	m_state_empty = false;

	return 0;
}

int C_Sender::Get_Frame_Buffer(C_Frame *frame) {

	// test for window empty:
	if(m_state_empty == true) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Sender::Get_Frame_Buffer() - attempted reading from empty window");
		#endif

		return (-1);
	}

	// copy the frame:
	*frame = *m_window[m_index_send];

	// set numbers:
	frame->Set_N(m_index_send,m_index_ackd);

	// update indexes:
	// sending:
	if((++m_index_send) == _SendingFrameWindow_Size) m_index_send = 0;

	// is the window empty (all sent), i.e. is adding possible?
	if(m_index_send == m_index_keys) m_state_empty = true;

	return 0;
}

void C_Sender::Update(C_Frame *frame) {

	// number of frame being acknowledged:
	byte Nr = frame->Get_Nr();

	#ifdef _Log_Type_Dump
	g_log->Log("\n C_Sender::Update() - received this acknowledging frame: ");
	frame->Dump();
	#endif

	switch(frame->Get_Header()) {

		case ACK:

			// is this the expected acknowledgement?
			if(Nr == m_index_ackd) {

				// ok, update indexes::

					#ifdef _Log_Type_Trace
					g_log->Log("\n C_Sender::Update() - received expected ACK, Nr = %d : ", Nr);
					#endif

					// acknowledging:
					if((++m_index_ackd) == _SendingFrameWindow_Size) m_index_ackd = 0;

					// the window can't be full anymore at this point,
					// at least one frame was just acknowledged and its place is now free
					if(m_state_full == true) m_state_full = false;

			}
			else {

				#ifdef _Log_Type_Error
				g_log->Log("\n C_Sender::Update() - received other ACK than expected: ");
				g_log->Log("\n C_Sender::Update() - expected Nr = %u, received Nr = %u", m_index_ackd, Nr);
				#endif
				// drop it - ignore it

			}

			break;

		case NAK:

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_Sender::Update() - received NAK, Nr = %d : ", Nr);
			#endif

			// set m_index_send back to the number of the failed frame (retransmit needed)
			m_index_send = Nr;

			// update indexes:
			// window definitely not empty now, there are data to send
			m_state_empty = false;

			break;

		default:

			#ifdef _Log_Type_Error
			g_log->Log("\n C_Sender::Update() - unexpected type of acknowledging frame");
			#endif

			break;
	}

	return;
}
