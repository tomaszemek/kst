// project: Communication protocol, v. 1.0
// module: kbd.h
// description: C_Kbd class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _KBD_H
#define _KBD_H

// project headers:
	#include "types.h"
	#include "event.h"
	#include "globvars.h"

// class C_Kbd encapsulates the keyboard,
// it gives access to the pressed key
class C_Kbd {

	public:
		C_Kbd();
		~C_Kbd();

		// was key pressed?
		bool Is_Event();

		// get the event (msg.code and kbd.code)
		void Get_Event(C_Event *event);

};

#endif // _KBD_H
