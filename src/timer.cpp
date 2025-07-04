// project: Communication protocol, v. 1.0
// module: timer.cpp
// description: C_Timer class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <dos.h>

// project headers:
	#include "timer.h"

C_Timer::C_Timer(int multiplier) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Timer::C_Timer() - instance construction started : ");
	#endif

	// real time clock increment source reference
	timer_source = (long int far *) MK_FP(0,0x46C);

	timer_start = timer_current = 0;
	timeout_multiplier = multiplier;

	running = false;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Timer::C_Timer() - instance construction completed");
	#endif

	return;
}

C_Timer::~C_Timer() {

	timer_source = NULL;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Timer::~C_Timer() - instance destruction completed");
	#endif

	return;
}

void C_Timer::Run() {

	timer_start = *timer_source;

	running = true;

	return;
}

bool C_Timer::Is_Over() {

	long int period;

	timer_current = *timer_source;

	period = timer_current - timer_start;

	// the source reference counter might have went through zero, 
	// so check for that and adapt if so
	if(period < 0) period = (-1) * period;

	// expired?
	if(period <= timeout_multiplier) return false;
	else {

		running = false;
		return true;
	}

}

