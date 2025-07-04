// project: Communication protocol, v. 1.0
// module: kst_fix.cpp
// description: module contaisn functionality to restore initial state 
//				if the program had crashed miserably earlier and had left 
//				the IPX socket and logfile opened and unable to be reopened again
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <stdio.h>
	#include <dos.h>

// project headers:
	#include "kst_fix.h"

int main()
{

	// try closing socket
	Close_Socket();

	// try closing log
	Close_Log();

	return 0;
}

void Close_Socket() {

	struct REGPACK r;

	r.r_bx = 1;				// service 'close socket'
	r.r_dx = _KST_Socket;	// socket number

	intr(_IPX_Interrupt_Number, &r);

	return;
}

void Close_Log() {

	flushall();

	fcloseall();

	return;
}