// project: Communication protocol, v. 1.0
// module: event.cpp
// description: C_Event class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	// none

// project headers:
	#include "event.h"
	#include "globvars.h"

C_Event::C_Event() {

	kbd.code = 0;
	kbd.ext_code = 0;

	sm.state = state_600;

	app.mode = mode_nothing;

	msg.code = msg_nothing;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Event::C_Event() - instance created OK");
	#endif

}

C_Event::~C_Event() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Event::~C_Event() - instance destructed OK");
	#endif

	return;
}

