// project: Communication protocol, v. 1.0
// module: error.cpp
// description: C_Error class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <stdio.h>
	#include <conio.h>
	#include <stdlib.h>

// project headers:
	#include "error.h"

C_Error::C_Error() {

	return;

}

C_Error::~C_Error() {

	return;
}

void C_Error::Error(const char *errmsg, ...) {

	va_list arguments;

	clrscr();

	va_start(arguments,errmsg);
	Warning(errmsg,arguments);
	va_end(arguments);

	exit(-1);
}

void C_Error::Warning(const char *warnmsg, ...) {

	va_list arguments;

	va_start(arguments,warnmsg);
	vfprintf(stderr,warnmsg,arguments);
	va_end(arguments);

	getch();
}
