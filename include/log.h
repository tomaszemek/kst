// project: Communication protocol, v. 1.0
// module: log.h
// description: C_Log class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Log_H
#define _Log_H

// system headers:
	#include <stdio.h>
	#include <stdarg.h>

// project headers:
	#include "types.h"
	#include "constant.h"

// universal logging functionality
// constructor opens a predefined file name
// the Log method accepts variable arguments count
// and prints them out
class C_Log {

	// log file
		FILE *log;

	// buffer to process variable arguments count
		byte buffer[_Log_Buffer_Size];

	public:

		C_Log();
		~C_Log();

		void Log(const char *str, ... );

	};

#endif