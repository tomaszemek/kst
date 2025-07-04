// project: Communication protocol, v. 1.0
// module: error.h
// description: C_Error class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Error_H
#define _Error_H

// system headers:
	#include <stdarg.h>

// class Error: handles various runtime exceptions (typically during memory management,
// lack of important prerequisities, etc.)
// Instance of this class is gloval across the whole program
// For the time being we print to stderr

class C_Error {

	public:

		C_Error();
		~C_Error();

		// prints a message and terminates program
		void Error(const char *errmsg, ...);

		// prints a message, waits for key press to confirm, 
		// won't terminate the programe
		void Warning(const char *warnmsg, ...);

};

#endif