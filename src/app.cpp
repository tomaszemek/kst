// project: Communication protocol, v. 1.0
// module: app.cpp
// description: C_App class definition
//				Application global variables definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <conio.h>
	#include <string.h>
	#include <ctype.h>
	#include <stdlib.h>

// project headers:
	#include "app.h"

// additional include:
	#ifdef _Log_Enabled
		#include <alloc.h>
	#endif

// global variables definition:
	// windows:
	global C_Desktop *g_desktop;
	global int g_hwnd_local;
	global int g_hwnd_remote;
	global int g_hwnd_status;
    global int g_hwnd_frame_monitor;

	// ID's of stations:
	global byte g_local_id;
	global byte g_remote_id;

// local variables:
	// priorities for C_App::Message_Loop()
	// 1 ... trigger state machine
	// 2 ... check for key pressed
	// 3 ... check for connection closed
	local byte l_priority_buffer[_App_Priority_Buffer_Size] =
		{
			1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,2,
			1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,3

		};

C_App::C_App() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - instance construction started : ");
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - about to create 'g_desktop' : ");
	g_log->Log("\n C_App::C_App() - free memory left = %lu B",farcoreleft());
	#endif

	g_desktop = new C_Desktop;

		if(g_desktop == NULL) g_error->Error("C_App::C_App() - 'g_desktop': error during memory allocation\n");

	g_hwnd_local = _No_Wnd;
	g_hwnd_remote = _No_Wnd;
	g_hwnd_status = _No_Wnd;
	g_hwnd_frame_monitor = _No_Wnd;

	g_local_id = 0;
	g_remote_id = 0;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - about to create 'm_kbd' : ");
	g_log->Log("\n C_App::C_App() - free memory left = %lu B",farcoreleft());
	#endif

	m_kbd = new C_Kbd;

		if(m_kbd == NULL) g_error->Error("C_App::C_App() - 'm_kbd': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - about to create 'm_commprot' : ");
	g_log->Log("\n C_App::C_App() - free memory left = %lu B",farcoreleft());
	#endif

	m_commprot = new C_Commprotocol;

		if(m_commprot == NULL) g_error->Error("C_App::C_App() - 'm_commprot': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - about to create 'm_event' : ");
	g_log->Log("\n C_App::C_App() - free memory left = %lu B",farcoreleft());
	#endif

	m_event = new C_Event;

		if(m_event == NULL) g_error->Error("C_App::C_App() - 'm_event': error during memory allocation\n");

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::C_App() - instance construction completed ");
	g_log->Log("\n C_App::C_App() - free memory left = %lu B",farcoreleft());
	#endif

	return;
}

C_App::~C_App() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - instance destruction started : ");
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - about to destruct 'm_event' : ");
	g_log->Log("\n C_App::~C_App() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_event;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - about to destruct 'm_commprot' : ");
	g_log->Log("\n C_App::~C_App() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_commprot;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - about to destruct 'm_kbd' : ");
	g_log->Log("\n C_App::~C_App() - free memory left = %lu B",farcoreleft());
	#endif

	delete m_kbd;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - about to destruct 'g_desktop' : ");
	g_log->Log("\n C_App::~C_App() - free memory left = %lu B",farcoreleft());
	#endif

	delete g_desktop;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::~C_App() - free memory left = %lu B",farcoreleft());
	g_log->Log("\n C_App::~C_App() - instance destruction completed");
	#endif

	return;
}

uint C_App::Menu() {

	uint retval;
	bool done = false;

	int hwnd_menu;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::Menu() - creating window : ");
	#endif

	hwnd_menu = g_desktop->Create_Window(Menu_Window,"Main menu",1,1,80,25);

		if(hwnd_menu == _No_Wnd) {

			g_error->Error("\n C_App::Menu() - failed to create window 'hwnd_menu'");

		}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_App::Menu() - window created");
	#endif

	g_desktop->Set_Current(hwnd_menu);

	while (done != true) {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_App::Menu() - going to show menu");
		#endif

		g_desktop->Clear_Window();

		g_desktop->Print_String("\n - program functions : ");
		g_desktop->Print_String("\n\n <a> .......... chat, active connection establish attempt");
		g_desktop->Print_String("\n <p> .......... chat, passive connection establish attempt");
		g_desktop->Print_String("\n <m> .......... frame monitor, network traffic trace");
		g_desktop->Print_String("\n <ESC> ........ quit the program");

		g_desktop->Print_String("\n\n - settings : ");
		g_desktop->Print_String("\n\n <i> .......... set local station ID");

		g_desktop->Print_String("\n\n\n\n your choice : ");

		m_kbd->Get_Event(m_event);

		switch(m_event->kbd.code) {

			case 'a':
			case 'A':

						retval = mode_active;
						done = true;
						break;

			case 'p':
			case 'P':

						retval = mode_passive;
						done = true;
						break;

			case 'i':
			case 'I':

						retval = mode_change_local_ID;
						done = true;
						break;

			case 'm':
			case 'M':

						retval = mode_frame_monitor;
						done = true;
						break;

			case kb_esc:

						retval = mode_quit;
						done = true;
						break;

			case kb_extended:

						break;

			default:	break;

		}

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_App::Menu() - user input received, choice made = '%u', retval = '%d'", m_event->kbd.code, (int) retval);
		#endif

	}

	g_desktop->Destruct_Window(hwnd_menu);

	return retval;
}

byte C_App::Get_Station_ID(const char *title) {

	// return value, station ID:
	int the_id;

	// loop control:
	bool done = false;
	bool completed = false;

	// buffer:
	byte buffer[_Max_X];
	int position;

	// title aligned:
	char *prompt_formatted = new char[_Title_Length];

	sprintf(prompt_formatted,"\n Please enter number in range 1 - %u ; ESC to skip and use 0 \n\n %s : ",_Max_Station_ID,title);

	// TODO: implement flexible window titles, as local and remote inquiry look just the same, yet it's different station 

	// create a window:
	int hwnd_getid = g_desktop->Create_Window(Input_Window,title,3,5,75,7);

	if(hwnd_getid == _No_Wnd) {

		g_error->Error("\n C_App::GetLocalID() - failed to create window '%s'",title);

	}

	// make it active:
	g_desktop->Set_Current(hwnd_getid);

	// loop until valid value for ID was provided
	while (done != true) {

		// reset buffer:
		for(int i=0;i<_Max_X;i++) buffer[i] = 0;
		position = 0;

		g_desktop->Clear_Window();
		g_desktop->Print_String(prompt_formatted);

		// input handling
		while (completed != true) {

			// get the key code
			m_kbd->Get_Event(m_event);

			switch(m_event->kbd.code) {

				case kb_enter:

					completed = true;
					done = true;

					break;

				case kb_backspace:

					if(position > 0) {

						buffer[position--] = 0;
						g_desktop->Print_Char(m_event->kbd.code);
					}

					break;

				case kb_esc:

					completed = true;
					done = false;

					break;

				case kb_tab:

					break;

				case kb_extended:

					break;

				default:

					if(isdigit(m_event->kbd.code)) {

						buffer[position++] = (byte) m_event->kbd.code;
						g_desktop->Print_Char(m_event->kbd.code);
					}

					break;
			}
		}

		if(done == true) {

			// process the input and look for valid value:
			sscanf(buffer,"%d",&the_id);

			if((the_id > 0) && (the_id <= _Max_Station_ID)) done = true;
			else {

				completed = false;
				done = false;
			}

		}
		else {

			// for ESC:
			done = true;
			the_id = 0;
		}

	}

	g_desktop->Destruct_Window(hwnd_getid);

	return (byte)the_id;
}

void C_App::Message_Loop() {

	// loop control:
	bool done = false;

	m_priority_index = 0;

	// main loop:
	while (done != true) {

		// which action has priority now?
		switch(l_priority_buffer[m_priority_index]) {

			case priority_1:

				// trigger state machine
				m_commprot->Process_Event(m_event);

				break;

			case priority_2:

				// check for key pressed
				if(m_kbd->Is_Event() == true) {

					#ifdef _Log_Type_Trace
					g_log->Log("\n C_App::Message_Loop() - key pressed");
					#endif

					// get the event:
					m_kbd->Get_Event(m_event);

					// process it within the state machine:
					m_commprot->Process_Event(m_event);
				}

				break;

			case priority_3:

				// check for connection closed
				if(m_commprot->Is_Event() == true) {

					// get the event:
					m_commprot->Get_Event(m_event);

					// process it within the application
					// (state machine has finished)
					Process_Event();

					done = true;
				}

				break;

			default:

				done = true;

				#ifdef _Log_Type_Error
				g_log->Log("\n C_App::Message_Loop() - unexpected priority value ");
				#endif

				#ifdef _Log_Type_Dump
				g_log->Log("\n C_App::Message_Loop() - dump: m_priority_index == '%d', l_priority_buffer[m_priority_index] = '%d'",m_priority_index, l_priority_buffer[m_priority_index]);
				#endif

				break;
		}

		// iterate / rotate priority index:
		if((++m_priority_index) == _App_Priority_Buffer_Size) m_priority_index = 0;
	}

	// event processed:
	m_event->msg.code = msg_nothing;

	return;
}

void C_App::Process_Event() {

	// switch on event source:
	switch(m_event->msg.code) {

		// state machine finished indication:
		case msg_sm_finished:

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_App::Process_Event() - connection closed, windows destruction started : ");
			#endif

			// switch on current window layout:
			switch(m_event->app.mode) {

				case mode_active:
				case mode_passive:

					// destruct windows:
					g_desktop->Destruct_Window(g_hwnd_remote);
					g_desktop->Destruct_Window(g_hwnd_local);
					g_desktop->Destruct_Window(g_hwnd_status);

					break;

				case mode_frame_monitor:

					// destruct window:
					g_desktop->Destruct_Window(g_hwnd_frame_monitor);

					break;

				default:

					#ifdef _Log_Type_Error
					g_log->Log("\n C_App::Process_Event() - unexpected application mode value : '%d'",m_event->app.mode);
					#endif

					break;
			}

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_App::Process_Event() - windows destruction completed ");
			#endif

			break;

		// other messages we don't care about:
		default:

			break;

	}	// end of switch (msg.code)

	// event processed:
	m_event->msg.code = msg_nothing;

	return;
}


int C_App::Run(int argc, char *argv[]) {

	// no arguments ?
	if(argc == 1) {

		// set local station ID:
		g_local_id = Get_Station_ID("This station ID");

		if(g_local_id == 0) {

			#ifdef _Log_Type_Error
			g_log->Log("\n C_App::Run() - failed to determine local station ID, using 0");
			#endif

		}
		else {

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_App::Run() - g_local_id == '%d'", (int) g_local_id);
			#endif

		}

		// reset event:
		m_event->msg.code = msg_nothing;

		// main loop:
		while (m_event->msg.code != msg_quit) {

			// what does user want to do?
			switch(m_event->app.mode = Menu()) {

				// active connection establish attempt:
				case mode_active:

					// with whom?
					g_remote_id = Get_Station_ID("Remote station ID");

					if(g_remote_id == 0) {

						#ifdef _Log_Type_Error
						g_log->Log("\n C_App::Run() - failed to determine remote station ID, using 0");
						#endif

					}

					// create windows:
					g_hwnd_status = g_desktop->Create_Window(Status_Window,"Status window",1,19,80,7);

						if(g_hwnd_status == _No_Wnd) g_error->Error("C_App::C_App() - failed to create window 'g_hwnd_status'\n");

					g_hwnd_local = g_desktop->Create_Window(Input_Window,"Local",1,1,80,9);

						if(g_hwnd_local == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_local'\n");

					g_hwnd_remote = g_desktop->Create_Window(Output_Window,"Remote",1,10,80,9);

						if(g_hwnd_remote == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_remote'");

					// set state machine accordingly:
					m_event->msg.code = msg_connect;
					m_event->sm.state = state_601;

					// start the state machine:
					m_commprot->Process_Event(m_event);

					// start the message loop:
					Message_Loop();

					// event processed:
					m_event->msg.code = msg_nothing;

					break;

				// active connection establish attempt:
				case mode_passive:

					// create windows:
					g_hwnd_status = g_desktop->Create_Window(Status_Window,"Status window",1,19,80,7);

						if(g_hwnd_status == _No_Wnd) g_error->Error("C_App::C_App() - failed to create window 'g_hwnd_status'\n");

					g_hwnd_local = g_desktop->Create_Window(Input_Window,"Local",1,1,80,9);

						if(g_hwnd_local == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_local'\n");

					g_hwnd_remote = g_desktop->Create_Window(Output_Window,"Remote",1,10,80,9);

						if(g_hwnd_remote == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_remote'");

					// set state machine accordingly:
					m_event->msg.code = msg_connect;
					m_event->sm.state = state_602;

					// start the state machine:					
					m_commprot->Process_Event(m_event);

					// start the message loop:
					Message_Loop();

					// event processed:
					m_event->msg.code = msg_nothing;

					break;

				// frame monitor:
				case mode_frame_monitor:

					g_hwnd_frame_monitor = g_desktop->Create_Window(Status_Window,"Frame Monitor",1,1,80,25);

						if(g_hwnd_frame_monitor == _No_Wnd) g_error->Error("C_App::C_App() - failed to create window 'g_hwnd_frame_monitor'\n");

					// set state machine accordingly:
					m_event->msg.code = msg_connect;
					m_event->sm.state = state_603;

					// start the state machine:
					m_commprot->Process_Event(m_event);

					// start the message loop:
					Message_Loop();

					// event processed:
					m_event->msg.code = msg_nothing;

					break;

				// change local station ID:
				case mode_change_local_ID:

					g_local_id = Get_Station_ID("This station ID");

					if(g_local_id == 0) {

						#ifdef _Log_Type_Error
						g_log->Log("\n C_App::Run() - failed to determine local station ID, using 0");
						#endif
					}

					// event processed:
					m_event->msg.code = msg_nothing;

					break;

				// quit program:
				case mode_quit:

					m_event->msg.code = msg_quit;

					break;

				// otherwise:
				default:

					// error
					#ifdef _Log_Type_Error
					g_log->Log("\n C_App::Run() - unknown return value from 'Menu()'");
					#endif

					// invoke Menu() on next iteration again:
					m_event->msg.code = msg_nothing;

					break;

			}	// end of switch(m_event->app.mode)

		}	// end of main loop

	}	// end of the 'no arguments' part
	else {

		switch(m_event->app.mode = Parse_Arguments(argc,argv)) {

			case mode_active:

				// create windows:
				g_hwnd_status = g_desktop->Create_Window(Status_Window,"Status window",1,19,80,7);

					if(g_hwnd_status == _No_Wnd) g_error->Error("C_App::C_App() - failed to create window 'g_hwnd_status'\n");

				g_hwnd_local = g_desktop->Create_Window(Input_Window,"Local",1,1,80,9);

					if(g_hwnd_local == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_local'\n");

				g_hwnd_remote = g_desktop->Create_Window(Output_Window,"Remote",1,10,80,9);

					if(g_hwnd_remote == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_remote'");

				// set state machine accordingly:
				m_event->msg.code = msg_connect;
				m_event->sm.state = state_601;

				// set station IDs using command line arguments:
				g_local_id = atoi(argv[_Argv_Local_ID]);
				g_remote_id = atoi(argv[_Argv_Remote_ID]);

				// start the state machine:
				m_commprot->Process_Event(m_event);

				// start the message loop:
				Message_Loop();

				// event processed:
				m_event->msg.code = msg_nothing;

				break;

			case mode_passive:

				// create windows:
				g_hwnd_status = g_desktop->Create_Window(Status_Window,"Status window",1,19,80,7);

					if(g_hwnd_status == _No_Wnd) g_error->Error("C_App::C_App() - failed to create window 'g_hwnd_status'\n");

				g_hwnd_local = g_desktop->Create_Window(Input_Window,"Local",1,1,80,9);

					if(g_hwnd_local == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_local'\n");

				g_hwnd_remote = g_desktop->Create_Window(Output_Window,"Remote",1,10,80,9);

					if(g_hwnd_remote == _No_Wnd) g_error->Error("C_App::Run() - failed to create window 'g_hwnd_remote'");

				// set state machine accordingly:
				m_event->msg.code = msg_connect;
				m_event->sm.state = state_602;

				// set station IDs using command line arguments:				
				g_local_id = atoi(argv[_Argv_Local_ID]);
				g_remote_id = 0;

				// start the state machine:
				m_commprot->Process_Event(m_event);

				// start the message loop:
				Message_Loop();

				// event processed:
				m_event->msg.code = msg_nothing;

				break;

			case mode_quit:

				// error
				#ifdef _Log_Type_Error
				g_log->Log("\n C_App::Run() - bad argument values");
				#endif

				break;

			default:

				// error
				#ifdef _Log_Type_Error
				g_log->Log("\n C_App::Run() - unknown return value from method 'Parse_Arguments()'");
				#endif

				break;
		}

	}

	return 0;
}

uint C_App::Parse_Arguments(int argc, char *argv[]) {

	uint retval;

	int local_id, remote_id;

	switch(argc) {

		case _Argc_Passive:

			local_id = atoi(argv[_Argv_Local_ID]);

			// test whether the conversion turned successfully:
			if(local_id == 0) retval = mode_quit;
			else {

				// test whether the value is in limits:
				if((local_id > 0) && (local_id <= _Max_Station_ID)) retval = mode_passive;
				else retval = mode_quit;

			}

			break;

		case _Argc_Active:

			local_id = atoi(argv[_Argv_Local_ID]);
			remote_id = atoi(argv[_Argv_Remote_ID]);

			// test whether the conversion turned successfully:
			if((local_id == 0) || (remote_id == 0)) retval = mode_quit;
			else {

				// test whether the values are in limits:
				if((local_id > 0) && (local_id <= _Max_Station_ID)) {

					if((remote_id > 0) && (remote_id <= _Max_Station_ID))

						retval = mode_active;

				}
				else retval = mode_quit;

			}

			break;

		default:

			retval = mode_quit;
			break;
	}

	return retval;
}
