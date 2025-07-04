// project: Communication protocol, v. 1.0
// module: commprot.cpp
// description: C_Commprotocol class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <mem.h>

// project headers:
	#include "commprot.h"

// additional includes:
	#ifdef _Log_Enabled
		#include <alloc.h>
	#endif

// global variables:
	global bool g_frame_monitor_mode;

// local variables of this module:
	// array of priority indicators, used for status net_20 condition selection
	local byte l_priority_buffer[_Commprotocol_Priority_Buffer_Size] =
	{
		4,3,1,2,1,4,3,1,2,1,
		3,2,1,4,2,4,3,1,2,1,
		3,2,4,3,1,2,4,3,1,2,
		1,3,2,4,4,3,1,2,7,3,
		1,2,1,3,4,2,1,3,1,2,
		1,3,1,2,4,3,1,2,6,3,
		1,2,1,3,4,2,1,3,1,2,
		1,3,1,2,4,3,1,2,5,3,
		1,2,1,3,4,2,1,3,1,2,
		1,3,1,2,4,3,1,2,8,3
	};

/*
// initial testing values: - length 72
	{	1,2,3,4,1,5,2,6,3,1,4,2,7,1,3,5,2,4,
		1,6,3,2,1,5,4,3,2,1,7,6,4,2,1,3,5,8,
		1,3,2,4,1,5,3,6,2,1,4,3,8,1,2,5,3,4,
		1,6,2,3,1,5,4,2,3,1,8,6,4,3,1,2,5,7 };
*/


// Actual communication protocol: class C_Commprotocol
C_Commprotocol::C_Commprotocol() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - instance construction started : ");
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_send_frame' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_send_frame = new C_Frame;

		if(m_send_frame == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_send_frame': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_recv_frame' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_recv_frame = new C_Frame;

		if(m_recv_frame == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_recv_frame': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_fcs' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_fcs = new C_FCS;

		if(m_fcs == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_fcs': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_timer_A' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_timer_A = new C_Timer(10);

		if(m_timer_A == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_timer_A': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_timer_B' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_timer_B = new C_Timer(36);

		if(m_timer_B == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_timer_B': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_timer_C' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_timer_C = new C_Timer(73);

		if(m_timer_C == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_timer_C': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_timer_D' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_timer_D = new C_Timer(50);

		if(m_timer_D == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_timer_D': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_sender' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_sender = new C_Sender;

		if(m_sender == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_sender': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_receiver' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_receiver = new C_Receiver;

		if(m_receiver == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_receiver': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_ipx' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_ipx = new C_IPX;

		if(m_ipx == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_ipx': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_keybuffer' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_keybuffer = new C_KeyBuffer;

		if(m_keybuffer == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_keybuffer': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - about to create 'm_frame_monitor_buffer' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	m_frame_monitor_buffer = new byte[_Frame_Monitor_Buffer_Size];

		if(m_frame_monitor_buffer == NULL) g_error->Error("C_Commprotocol::C_Commprotocol() - 'm_frame_monitor_buffer': error during memory allocation\n");

	// initialize priority indicator index:
	m_priority_index = 0;

	// initialize state related variables:
	m_connecting_counter = 0;
	m_state = state_600;

	// indicator that state machine has finished:
	m_finished = false;

	// frame monitor mode:
	g_frame_monitor_mode = false;

	// reset count of ENQ's:
	m_count_ENQ = 0;

	// initialization completed:
	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - instance initialization complete");
	#endif

}

C_Commprotocol::~C_Commprotocol() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - instance destruction started : ");
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_frame_monitor_buffer' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_frame_monitor_buffer;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_keybuffer' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_keybuffer;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_ipx' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_ipx;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_receiver' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_receiver;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_sender' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_sender;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_timer_D' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_timer_D;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_timer_C' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_timer_C;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_timer_B' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_timer_B;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_timer_A' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_timer_A;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_fcs' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_fcs;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_recv_frame' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_recv_frame;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - about to destruct 'm_send_frame' : ");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_send_frame;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Commprotocol::~C_Commprotocol() - instance destruction complete");
	g_log->Log("\n C_Commprotocol::C_Commprotocol() - free memory left = %lu B",farcoreleft());
	#endif

}

void C_Commprotocol::Change_State(uint new_state) {

	#ifdef _Log_Type_State	
	uint old_state = m_state;
	#endif

	m_state = new_state;

	#ifdef _Log_Type_State
	g_log->Log("\n State '%d' changed to '%d'",old_state,new_state);
	#endif

}

void C_Commprotocol::Process_Event(C_Event *event) {

	// which message is brought with the event?
	switch(event->msg.code) {

		// it is a regular key:
		case msg_getkey:

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_Commprotocol::Process_Event() - code msg_getkey: kbd.code = %u, adding to keybuffer", event->kbd.code);
			#endif

			// store the key to keybuffer if there is room left
			if(m_keybuffer->Is_Full() == false)

				m_keybuffer->Add_Key(event->kbd.code);

			else {

				g_desktop->Print_String_To_Window(g_hwnd_status,"Keybuffer full");

				#ifdef _Log_Type_Error
				g_log->Log("\n C_Commprotocol::Process_Event() - msg_getkey: keybuffer full");
				#endif

			}

			// message processed:
			event->msg.code = msg_nothing;

			break;

		// it is an extended key (one such that on first getch() call they return 0 and on second the actual code)
		case msg_getextkey:

			// no use of those:
			#ifdef _Log_Type_Trace
			g_log->Log("\n C_Commprotocol::Process_Event() - code msg_getextkey: kbd.extcode = %u, ignoring the key", event->kbd.ext_code);
			#endif

			// message processed:
			event->msg.code = msg_nothing;

			break;

		// connection shall be established
		case msg_connect:

			// initial state:
			m_state = event->sm.state;

			event->msg.code = msg_nothing;

			break;

		// no specific message
		case msg_nothing:

			// perform action corresponding to current state:
			switch(m_state) {

				case state_600:

					// no connection

					// no action to be taken at this state

					break;

				case state_601:

					// active connection mode - initialize before start connection setup attempt

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_601', initializing");
					#endif

					g_desktop->Set_Current(g_hwnd_local);
					g_desktop->Print_String_To_Window(g_hwnd_status,"Establishing connection");

					m_keybuffer->Empty();

					m_sender->Init();
					m_receiver->Init();

					m_ipx->Start();

					m_connecting_counter = 0;

					Change_State(state_610);

					break;

				case state_602:

					// passive connection mode - initialize before connection setup attempt

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_602', initializing");
					#endif

					g_desktop->Set_Current(g_hwnd_local);
					g_desktop->Print_String_To_Window(g_hwnd_status,"Waiting for connection setup attempts ...");

					m_keybuffer->Empty();

					m_sender->Init();
					m_receiver->Init();

					// in the passive connection setup mode ipx needs to know only local station ID;
					// all frames are evaluated and first received ENQ will be used to setup connection
					m_ipx->Start();

					Change_State(state_612);

					break;

				case state_603:

					// frame monitor mode - initialize before monitor starts running

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_603', initializing");
					#endif

					g_desktop->Set_Current(g_hwnd_frame_monitor);

					m_keybuffer->Empty();

					// announce to IPX that promiscuous mode is on
					g_frame_monitor_mode = true;

					m_ipx->Start();

					g_desktop->Print_String("Frame monitor active, press ESC to quit");

					Change_State(state_640);

					break;

				case state_610:

					// active attempt to establish connection, start an attempt:

					// send ENQ
					// restart timer_A
					// print dot

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_610', constructing and sending ENQ frame : ");
					#endif

					m_send_frame->Set(ENQ);
					m_ipx->Send(m_send_frame);

					m_timer_A->Run();

					char *str_dot = new char[_Max_String_Length];
					int i = 0;

					for(; i<m_count_ENQ; i++) {

						str_dot[i] = ' '; // TODO - convert to kb_space constant

					}

					str_dot[i++] = '.'; // the dot
					str_dot[i] = '\0';	// end of string


					g_desktop->Print_String_To_Window(g_hwnd_status,str_dot);

					m_count_ENQ++;

					Change_State(state_611);

					break;

				case state_611:

					// active attempt to establish connection, determine network/keyboard input after start:

					// timer_A expired -> state_614
					// ENQ received -> state_613
					// ACK received -> state_615
					// ESC pressed -> state_660

					// we are interested in just one condition at a time
					// there could be more of them true but we can't act on all at the same time,
					// so we prioritize the condition testing based on probability, relevance and impact
					// similar approach used by other multiple condition testing in other states

					// 1st condition: timer expired?
					if(m_timer_A->Is_Over() == true) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_611', timer_A expired");
						#endif

						Change_State(state_614);
					}
					else {

						// 2nd condition: ENQ received?
						if(m_ipx->Ready(ENQ) == true) {

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_611', ENQ received");
							#endif

							m_ipx->Get(m_recv_frame);

							#ifdef _Log_Type_Dump
							m_recv_frame->Dump();
							#endif

							Change_State(state_613);
						}
						else {

							// 3rd condition: ACK received?
							if(m_ipx->Ready(ACK) == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_611', ACK received");
								#endif

								m_ipx->Get(m_recv_frame);

								#ifdef _Log_Type_Dump
								m_recv_frame->Dump();
								#endif

								Change_State(state_615);
							}
							else {

								// 4th condition: ESC pressed
								if(m_keybuffer->Is_Key(kb_esc) == true) {

									#ifdef _Log_Type_State
									g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_611', ESC pressed");
									#endif

									m_keybuffer->Get_Key();

									Change_State(state_660);
								}

							}	// end of 3rd else
						}   // end of 2nd else
					}	// end of 1st else

					// other keys pressed in this state are dropped:
					if(m_keybuffer->Is_Empty() == false) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_611', pressed any other key, dropped");
						#endif

						m_keybuffer->Get_Key();

						// no state change here
					}

					break;

				case state_612:

					// passive connection setup attempt, determine network/keyboard input:

					// ENQ received -> state_613
					// ESC pressed -> state_660

					if(m_ipx->Ready(ENQ) == true) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_612', ENQ received");
						#endif

						m_ipx->Get(m_recv_frame);

						#ifdef _Log_Type_Dump
						m_recv_frame->Dump();
						#endif

						// remote station ID
						g_remote_id = m_recv_frame->Get_Sender();

						// test if it falls into limits (1 - _Max_Station_ID):
						if((g_remote_id == 0) || (g_remote_id > _Max_Station_ID)) {

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_612', remote station ID is invalid : '%d'",g_remote_id);
							#endif

							g_desktop->Print_String_To_Window(g_hwnd_status,"Remote station ID is invalid");

							Change_State(state_690);	// runtime error
						}
						else {

							// remote ID is valid

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_612', remote station ID = %d", g_remote_id);
							#endif

							Change_State(state_613);
						}

					}
					else {

						// test for ESC
						if(m_keybuffer->Is_Key(kb_esc) == true) {

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_612', ESC pressed");
							#endif

							m_keybuffer->Get_Key();

							Change_State(state_660);
						}

					}

					// other keys pressed in this state are dropped
					if(m_keybuffer->Is_Empty() == false) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_612', pressed any other key, dropped");
						#endif

						m_keybuffer->Get_Key();

						// no state change here
					}

					break;

				case state_613:

					// ENQ received, respond to that:

					// send ACK

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_613', constructing and sending ACK as response to ENQ");
					#endif

					m_send_frame->Set(ACK);
					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					Change_State(state_615);

					break;

				case state_614:

					// timer_A expired, respond to that:

					// increment connection setup attempt counter, compare with limit:
					// counter <= limit -> state_610
					// counter > limit -> state 661

					m_connecting_counter++;

					if(m_connecting_counter <= _Max_Connect_Retries) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_614', connection setup attempt counter = %u", m_connecting_counter);
						#endif

						Change_State(state_610);
					}
					else {

						#ifdef _Log_Type_Error
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_614', all connection setup attempts failed");
						#endif

						m_connecting_counter = 0;
						Change_State(state_661);
					}

					break;

				case state_615:

					// connection established, respond to that:

					// restart timer_B
					// restart timer_C

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection established");

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_615', connection established, restarting timer_B and timer_C");
					#endif

					m_timer_B->Run();
					m_timer_C->Run();

					Change_State(state_620);

					break;

				case state_620:

					// connection established, determine network/keyboard/timers input state:

						// TODO: optimize array of priority indicators

					// frame of type DAT, ACK, NAK received -> state_622
					// frames waiting in sending window -> state_624
					// frames waiting in receiving window -> state_627
					// keys pressed -> state_623
					// timer_B expired -> state_625
					// timer_C expired -> state_664
					// ESC pressed -> state_650
					// EOT received? -> state_626

					#ifdef _Log_Type_Dump
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority = %u", l_priority_buffer[m_priority_index]);
					#endif

					// get priority indicator and perform action corresponding to that priority:
					switch(l_priority_buffer[m_priority_index]) {

						case priority_1:

							// incoming frame of (DAT/ACK/NAK)?
							if((m_ipx->Ready(DAT) == true) || (m_ipx->Ready(ACK) == true) || (m_ipx->Ready(NAK) == true)) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_1, frame received (DAT/ACK/NAK)");
								#endif

								#ifdef _Log_Type_Dump
								m_recv_frame->Dump();
								#endif

								Change_State(state_622);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_1, no relevant network data");
								#endif

							}

							break;

						case priority_2:

							// frames waiting in sender?
							if(m_sender->Is_Empty() == false) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_2, frames waiting in sender");
								#endif

								Change_State(state_624);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_2, no frames waiting in sender");
								#endif

							}

							break;

						case priority_3:

							// acknowledging frames waiting in receiver?
							if(m_receiver->Need_Send() == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_3, acknowledging frames waiting in receiver to be sent");
								#endif

								Change_State(state_627);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_3, no acknowledging frames waiting in receiver");
								#endif

							}

							break;

						case priority_4:

							// keys pressed (except for ESC)
							if((m_keybuffer->Is_Empty() == false) && (m_keybuffer->Is_Key(kb_esc) == false)) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_4, keys pressed");
								#endif

								Change_State(state_623);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_4, no keys pressed or ESC pressed");
								#endif

							}

							break;

						case priority_5:

							// timer_B expired?
							if(m_timer_B->Is_Over() == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_5, timer_B expired");
								#endif

								Change_State(state_625);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_5, timer_B still running");
								#endif

							}

							break;

						case priority_6:

							// timer_C expired?
							if(m_timer_C->Is_Over() == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_6, timer_C expired");
								#endif

								Change_State(state_664);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_6, timer_C still running");
								#endif

							}

							break;

						case priority_7:

							// ESC pressed?
							if(m_keybuffer->Is_Key(kb_esc) == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_7, ESC pressed");
								#endif

								m_keybuffer->Get_Key();

								Change_State(state_650);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_7, ESC not waiting in keybuffer on top");
								#endif

							}

							break;

						case priority_8:

							// EOT received?
							if(m_ipx->Ready(EOT) == true) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_8, EOT received");
								#endif

								Change_State(state_626);

							}
							else {

								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_620', priority_8, EOT not received");
								#endif

							}

							// Here we need to break, as this was the last valid case before default.
							break;

						default:

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - unsupported priority indicator");
							g_log->Log("\n C_Commprotocol::Process_Event() - m_priority_index == %d, priority indicator value = %d",m_priority_index,l_priority_buffer[m_priority_index]);
							#endif

							Change_State(state_690);	// runtime error

							break;

					}

					// move to next priority indicator:
					if((++m_priority_index) == _Commprotocol_Priority_Buffer_Size) m_priority_index = 0;

					break;

				case state_621:

					// frame sent, respond to that:

					// restart timer_B

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_621', restarting timer_B");
					#endif

					m_timer_B->Run();

					Change_State(state_620);

					break;

				case state_622:

					// frame received, respond to that:

					// restart timer_C

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_622', restarting timer_C");
					#endif

					m_timer_C->Run();

					Change_State(state_630);

					break;

				case state_623:

					// keys pressed, waiting in keybuffer, respond to that:

					// if sender window full, return to state_620 and wait
					// otherwise store key to sender window queue and show it on screen

					if(m_sender->Is_Full() == true) {

						#ifdef _Log_Type_Error
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_623', sending window full");
						#endif

						g_desktop->Print_String_To_Window(g_hwnd_status,"Sending window full");

						Change_State(state_620);

					}
					else {

						uint the_key = m_keybuffer->Get_Key();

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_623', storing key '%c' to sending window and showing it on screen", (byte) the_key);
						#endif

						m_sender->Add_Char(the_key);

						g_desktop->Print_Char(the_key);

						// next state shall be state_624 (send frame)
						// but for general responsiveness it turns more effective to return to state_620:
						// but during testing the window full error is shown quite often so maybe it still needs tweaking and possibly return back to state_624 as next
						Change_State(state_620);

					}

					break;

				case state_624:

					// waiting frames in sender, respond to that:

					// construct DAT from the key (character) on the top of the sending queue and sent it

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_624', sending waiting DAT frame");
					#endif

					m_sender->Get_Frame_Buffer(m_send_frame);

					m_fcs->Create_FCS(m_send_frame);

					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					Change_State(state_621);

					break;

				case state_625:

					// timer_B expired, respond to that:

					// send 'heartbeat' frame (ACK(0,0))
					// this 'hearbeat' frame needs to be somehow differentiated from 'acknowledging' ACK's
					// as they seem to both look alike

					// I chose to implement it as such that the 'heartbeat' ACK's will carry specific FCS signature
					// and it only appears in states which work on such an 'heartbeat' ACK, being state_625 and state_636

					// I chose this modification: FCS = 0xFFFF

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_625', sending heartbeat frame ACK(0,0)");
					#endif

					m_send_frame->Set(ACK);

					// tag it:
					m_send_frame->Set_FCS(0xFFFF);

					m_ipx->Send(m_send_frame);

					Change_State(state_621);

					break;

				case state_626:

					// EOT received, respond to that:

					// send ACK:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_626', constructing and sending ACK as response to EOT");
					#endif

					m_ipx->Get(m_recv_frame);

					m_send_frame->Set(ACK);
					m_ipx->Send(m_send_frame);

					Change_State(state_665);

					break;

				case state_627:

					// waiting frames (acknowledgements) in receiver that should be sent, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_627', constructing and sending ackowledging frame");
					#endif

					m_receiver->Get_Reply(m_send_frame);
					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					Change_State(state_621);

					break;

				case state_630:

					// frame received, respond to that:

					// DAT -> state_631
					// ACK -> state_636
					// NAK -> state_637

					m_ipx->Get(m_recv_frame);

					switch(m_recv_frame->Get_Header()) {

						case DAT:

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_630', received DAT ");
							#endif

							Change_State(state_631);

							break;

						case ACK:

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_630', received ACK ");
							#endif

							Change_State(state_636);

							break;

						case NAK:

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_630', received NAK ");
							#endif

							Change_State(state_637);

							break;

						default:

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_630', received unsupported frame type ");
							#endif

							Change_State(state_690);		// runtime error

							break;

					}

					break;

				case state_631:

					// received DAT, respond to that:

					// FCS check OK? -> state_632
					// FCS check not OK? -> state_635

					if(m_fcs->Check_FCS(m_recv_frame) == true) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_631', FCS OK ");
						#endif

						Change_State(state_632);
					}
					else {

						#ifdef _Log_Type_Error
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_631', FCS not OK ");
						#endif

						Change_State(state_635);
					}

					break;

				case state_632:

					// received DAT, FCS OK, respond to that:

					// frame in sequence -> state_633
					// frame out of sequence -> state_634

					if(m_receiver->Verify(m_recv_frame) == true) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_632', frame in sequence ");
						#endif

						Change_State(state_633);
					}
					else {

						if(m_receiver->Is_Full() == true) {

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_632', receiving window full ");
							#endif
							
							Change_State(state_666);
						}
						else {

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_632', frame out of sequence ");
							#endif

							Change_State(state_634);
						}
					}

					break;

				case state_633:

					// received DAT, FCS OK, in sequence, respond to that:

					// send acknowledging ACK for this DAT back
					// show character (string) on screen in window 'remote'

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_633', construct and send acknowledging ACK for received DAT ");
					#endif

					m_receiver->Get_Reply(m_send_frame);
					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_633', show received character on the screen ");
					#endif

					uint the_key = (uint) m_recv_frame->Get_Character();

					g_desktop->Print_Char_To_Window(g_hwnd_remote,the_key);

					Change_State(state_621);

					break;

				case state_634:

					// received DAT, FCS OK, out of sequence, respond to that:

					// send waiting response NAK
					// it doesn't necessarily need to be acknowledgement for the received frame,
					// because frames can get lost/dropped on the way and it's necessary to follow
					// the sender needs (what it needs to confirm/get confirmed) at this moment

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_634', frame received out of sequence, sending response ");
					#endif

					m_receiver->Get_Reply(m_send_frame);
					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					Change_State(state_621);

					break;

				case state_635:

					// received DAT, FCS not OK, respond to that:

					// send respective NAK

					// TODO: should this be somehow made part of sender or receiver class instead?

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_635', constructing NAK for frame with incorrect FCS ");
					#endif

					// in m_recv_frame is the frame that failed FCS check:
					m_send_frame->Set(NAK);
					m_send_frame->Set_N(0,m_recv_frame->Get_Ns());

					m_ipx->Send(m_send_frame);

					#ifdef _Log_Type_Dump
					m_send_frame->Dump();
					#endif

					Change_State(state_621);

					break;

				case state_636:

					// ACK received, respond to that:

					// filter out frames with FCS == 0xFFFF
					// these are custom made heartbeat signature frames
					if(m_recv_frame->Get_FCS() == 0xFFFF) {

						g_desktop->Print_String_To_Window(g_hwnd_status,"Received heartbeat ACK");

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_636', received heartbeat ACK ");
						#endif

						Change_State(state_620);

					}
					// else it's acknowledging ACK:
					else {

						// Nr in limits?
						if(m_recv_frame->Get_Nr() >= _SendingFrameWindow_Size) {

							g_desktop->Print_String_To_Window(g_hwnd_status,"Invalid Nr received");

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_636', ACK frame contains invalid Nr = %u", m_recv_frame->Get_Nr());
							#endif

							// TODO: shall the frame be removed from the window at this point?

							Change_State(state_620);
						}
						else {

							// solve the problem that 0 as Nr can be as well as acknowledgement as heartbeat frame
							// solution - send it to update - TODO

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_636', ACK frame given to sender to update indexes ");
							#endif

							m_sender->Update(m_recv_frame);

							Change_State(state_620);

						}
					}

					break;

				case state_637:

					// NAK received, respond to that:

					// update indexes in sender window

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_637', NAK frame given to sender to update indexes ");
					#endif

					m_sender->Update(m_recv_frame);

					Change_State(state_620);

					break;

				case state_640:

					// frame monitor mode, determine network/keyboard input after start:

					if(m_ipx->Ready() == true) {

						m_ipx->Get(m_recv_frame);

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_640', frame received");
						#endif
						
						#ifdef _Log_Type_Dump
						m_recv_frame->Dump();
						#endif

						Change_State(state_641);
					}
					else {

						// ESC ?
						if(m_keybuffer->Is_Key(kb_esc) == true) {

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_640', ESC pressed");
							#endif

							m_keybuffer->Get_Key();
							
							Change_State(state_672);

						}
						else {

							// any other key?
							if(m_keybuffer->Is_Empty() == false) {

								#ifdef _Log_Type_State
								g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_640', any other key pressed, dropped");
								#endif

								m_keybuffer->Get_Key();

								// don't change state

							}
						}
					}

					break;

				case state_641:

					// frame received, show it:

					// prepare memory for a frame monitor frame buffer:
					_fmemset((void far *)m_frame_monitor_buffer,'\0',(_Frame_Monitor_Buffer_Size - 1));

					// print attributes to it:
					m_recv_frame->Dump(m_frame_monitor_buffer);

					// show it:
					g_desktop->Print_String(m_frame_monitor_buffer);

					Change_State(state_640);

					break;

				case state_650:

					// ESC pressed while connection established, respond to that:

					// send EOT
					// restart timer_D
					// show 'Closing connection'

					// TODO: here might possibly be also flush of all buffers and windows,
					// so that we can notice and process the corresponding ACK(0,0) as well

					m_send_frame->Set(EOT);

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_650', sending EOT");
					#endif

					m_ipx->Send(m_send_frame);

					m_timer_D->Run();

					g_desktop->Print_String_To_Window(g_hwnd_status,"Closing connection ...");

					Change_State(state_651);

					break;

				case state_651:

					// Closing connection, determine network/keyboard/timer input state

					// ACK received -> state_662
					// timer_D expired -> state_663

					if(m_ipx->Ready(ACK) == true) {

						#ifdef _Log_Type_State
						g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_651', ACK received");
						#endif

						m_ipx->Get(m_recv_frame);
						
						// needs to be 0,0:
						if(m_recv_frame->Get_Nr() == 0) {

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_651': received expected ACK");
							#endif

							Change_State(state_662);

						}
						else {

							#ifdef _Log_Type_Error
							g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_651': received different ACK than expected");
							#endif
							
							#ifdef _Log_Type_Dump
							m_recv_frame->Dump();
							#endif
						}

					}
					else {

						if(m_timer_D->Is_Over() == true) {

							#ifdef _Log_Type_State
							g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_651': timer_D expired");
							#endif

							Change_State(state_663);

						}

					}

					break;

				case state_660:

					// establishing connection, ESC pressed, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_660': connection setup attempt aborted by user ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection setup attempt aborted by user");

					Change_State(state_670);

					break;

				case state_661:

					// establishing connection, all attempts for timer_A failed, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_661': connection setup attempt aborted by timeout ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection setup attempt aborted by timeout");

					Change_State(state_670);

					break;

				case state_662:

					// connection established, ESC pressed, acknowledging ACK received, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_662': connection closed by user, disconnect request to remote peer confirmed ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection closed by user, disconnect request to remote peer confirmed");

					Change_State(state_670);

					break;

				case state_663:

					// connection established, ESC pressed, timer_D expired, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_663': connection closed by user, disconnect request to remote peer timed out ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection closed by user, disconnect request to remote peer timed out");

					Change_State(state_670);

					break;

				case state_664:

					// connection established. timer_C expired, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_664': remote peer not responding in time, connection aborted ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Remote peer not responding in time, connection aborted");

					Change_State(state_670);

					break;

				case state_665:

					// connection established, received EOT, sent ACK, respond to that:

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_665': connection termination requested by remote peer confirmed ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection termination requested by remote peer confirmed");

					Change_State(state_670);

					break;

				case state_666:

					// receiving window full

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_666': connection aborted on receiving window full ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection aborted on receiving window full");

					Change_State(state_670);

					break;

				case state_667:

					// sending window full

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_667': connection aborted on sending window full ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection aborted on sending window full");

					Change_State(state_670);

					break;

				case state_670:

					// connection closed, respond to that

					// Before waiting for ESC, a prompt shall be shown
					// it can't be shown in the same loop with waiting for key, 
					// as that would flood the logs and screen.

					// So rather show it once in a step in between

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_670': connection closed, showing prompt to press ESC to quit ");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Connection closed, press ESC to quit");

					Change_State(state_671);

					break;

				case state_671:

					// wait for ESC to quit

					uint confirmation;

					// wait for ESC:
					if(m_keybuffer->Is_Empty() == false) {

						confirmation = m_keybuffer->Get_Key();

						if(confirmation == kb_esc) Change_State(state_672);

					}
					// else stay in this state

					break;

				case state_672:

					// cleanup the communication protocol

					#ifdef _Log_Type_State
					g_log->Log("\n C_Commprotocol::Process_Event(), state 'state_672': cleanup after connection closed ");
					#endif

					m_finished = true;

					m_count_ENQ = 0;

					g_frame_monitor_mode = false;

					m_ipx->Stop();

					Change_State(state_600);

					break;

				case state_690:

					// runtime error, respond to that:

					#ifdef _Log_Type_Error
					g_log->Log("\n C_Commprotocol::Process_Event() - state 'state_690', unexpected situation occured");
					#endif

					g_desktop->Print_String_To_Window(g_hwnd_status,"Unexpected situation occured");

					Change_State(state_670);

					break;

				default:

					// unexpected state:

					#ifdef _Log_Type_Error
					g_log->Log("\n C_Commprotocol::Process_Event() - unexpected state = '%d'",m_state);
					#endif

					Change_State(state_690);

					break;

			}	// end of switch(m_state)

			break;

		// other messages are not handled, reset message code:
		default:

			#ifdef _Log_Type_State
			g_log->Log("\n C_Commprotocol::Process_Event(), unexpected message code = '%u'", event->msg.code);
			#endif
					
			event->msg.code = msg_nothing;
			break;

	}	// end of switch(event->msg.code)

	return;
}