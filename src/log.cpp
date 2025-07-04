// project: Communication protocol, v. 1.0
// module: log.cpp
// description: C_Log class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <stdio.h>

// project headers:
	#include "log.h"
	#include "error.h"

// referenced global variables
	import C_Error *g_error;

// local variables:
	// log file name:
	local const char *_Log_Filename = "log.log";

C_Log::C_Log() {

	#ifdef _Log_Enabled

	log = fopen(_Log_Filename,"wt");

		if(log == NULL) g_error->Error("C_Log::Open() - error when opening logfile");

	Log("\n C_Log::C_Log() - log opened");

	#endif

	return;

}

C_Log::~C_Log() {

	#ifdef _Log_Enabled

	Log("\n C_Log::~C_Log() - log closed");

	fclose(log);

	#endif

	return;
}

void C_Log::Log(const char *str, ... ) {

	#ifdef	_Log_Enabled

	va_list arguments;

	va_start(arguments,str);
	vfprintf(log,str,arguments);
	va_end(arguments);

	fflush(log);

	#endif

	return;
}

