// project: Communication protocol, v. 1.0
// module: app.h
// description: C_App class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _App_H
#define _App_H

// project headers:
	#include "types.h"		// standard types used in the program
	#include "event.h"		// event - to exchange information within the program
	#include "kbd.h"		// user interface - keyboard input
	#include "desktop.h"	// user interface - screen output
	#include "commprot.h"	// communication protocol state machine
	#include "globvars.h"	// global variables of the program

// application class:
// - represents the application - instance of this class owns the 
//	 input/output objects and performs event collection and distribution
// - it runs the high level flow of requesting local station ID
//   and prompting user to either start a chat with another station
//   or to run a frame monitor to show the network traffic between stations
class C_App {

	// keyboard input
		C_Kbd *m_kbd;

	// communication protocol state machine
		C_Commprotocol *m_commprot;

	// current event
		C_Event *m_event;

	// priorities index, used in Message_Loop()
		uint m_priority_index;

	// menu of actions, returns ID of the selected one
		uint Menu();

	// determine ID of local or remote station:
	// - title is unformatted title of the prompt window and 
	//		also used as a base to create a prompt string to explain 
	//		user what is the expected input
	// - returns provided ID
		byte Get_Station_ID(const char *title);

	// main message loop:
		void Message_Loop();

	// handle 'msg_sm_finished' - flag of state machine completed
		void Process_Event();

	// proces commandline arguments:
		uint Parse_Arguments(int argc, char *argv[]);

	public:

		C_App();
		~C_App();

	// run the application logic flow
		int Run(int argc, char *argv[]);
};

#endif // _App_H