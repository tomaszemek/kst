// project: Communication protocol, v. 1.0
// module: receiver.cpp
// description: C_Receiver class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	// none used

// project headers:
	#include "receiver.h"

// C_Receiver methods:
C_Receiver::C_Receiver() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Receiver::C_Receiver() - instance construction started : ");
	#endif

	Init();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Receiver::C_Receiver() - instance construction completed");
	#endif

	return;
}

C_Receiver::~C_Receiver() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Receiver::~C_Receiver() - instance destruction completed");
	#endif

	return;
}

void C_Receiver::Init() {

	// default values:
	m_index_expected = 0;
	m_index_send = 0;

	m_bool = false;

	// initialize the acknowledging window with NAKs:
	for(int i=0;i<_SendingFrameWindow_Size;i++) m_reply_buffer[i] = NAK;

	return;
}

bool C_Receiver::Verify(C_Frame *frame) {

	bool retval;

	// load values from the received frame:
	m_recv_Ns = frame->Get_Ns();
	m_recv_Nr = frame->Get_Nr();

	// test if received frame is in order
	// and set at position equal to its number (= Ns) an indication about
	// what should be sent back 
	if(m_recv_Ns == m_index_expected) {

		// => frame in order, response (acknowledgement) will be = ACK
		m_reply_buffer[m_index_expected] = ACK;

		// received frame is in order, update the index
		if((++m_index_expected) == _SendingFrameWindow_Size) m_index_expected = 0;

		retval = true;
	}
	else {

		// => frame out of order, response (acknowledgement) will be = NAK
		m_reply_buffer[m_index_expected] = NAK;

		// no index update as the expected frame hasn't been received yet

		retval = false;

	}

	// determine for which received frame the acknowledgement will be sent,
	// whether for the currently received one, or for another one received earlier,
	// but for the remote station not acknowledged yet (maybe ACK hasn't arrive yet)

	// this is the acknowledgement wanted / desired by the sender:
	m_index_send = m_recv_Nr;

	return retval;
}

void C_Receiver::Get_Reply(C_Frame *frame) {

	// prepare response (acknowledgement) frame,
	// as per the respective indication prepared by Verify()
	frame->Set(m_reply_buffer[m_index_send]);

	// number (of a received frame) being acknowledged:
	frame->Set_N(0,m_index_send);

	// is the number of the wanted / desired one equal to the last received one?
	if(m_index_send == m_recv_Ns) {

		// nothing left to send here:
		if(m_bool == true) m_bool = false;
	}
	else {

		// at least one more acknowledgement left to send:
		// set the state:
		if(m_bool == false) m_bool = true;

		// increment the wanted / desired index:
		if((++m_index_send) == _SendingFrameWindow_Size) m_index_send = 0;
	}

	return;
}