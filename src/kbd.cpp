// project: Communication protocol, v. 1.0
// module: kbd.cpp
// description: C_Kbd class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <conio.h>

// project headers:
	#include "kbd.h"
	#include "globvars.h"

C_Kbd::C_Kbd() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Kbd::C_Kbd() - instance construction completed");
	#endif

	return;
}

C_Kbd::~C_Kbd() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Kbd::~C_Kbd() - instance destruction completed");
	#endif

	return;
}

bool C_Kbd::Is_Event() {

	if(kbhit()) return true;
	else return false;
}

void C_Kbd::Get_Event(C_Event *event) {

	// read is performed from keyboard, not from standard input
	// note : getchar() didn't work here as expected

	event->kbd.code = getch();	// ... character of the pressed key

	// extended key code?
	if(event->kbd.code == kb_extended) {

		event->kbd.ext_code = getch();
		event->msg.code = msg_getextkey;

	}
	else event->msg.code = msg_getkey;

	return;
}