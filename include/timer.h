// project: Communication protocol, v. 1.0
// module: timer.h
// description: C_Timer class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Timer_H
#define _Timer_H

// project headers:
	#include "types.h"
	#include "globvars.h"

// passive timer / counter, can be started and tested for expiration,
// not actively running
// it works based on the individual computer 'ticks' - every 55 miliseconds
class C_Timer {

	// real time clock increment source reference
		long int far *timer_source;

	// data structure
		long timer_start, timer_current;
		int timeout_multiplier;	// = multiplier

		bool running;

	public:

	// constructor / destructor
		// sets the timeout - time for which the timer will be running (passively)
		// timeout is entered as a multiplier
		// actual timeout value is 'multiplier' * 55 miliseconds
		C_Timer(int multiplier = 0);

		~C_Timer();

		void Run();

		bool Is_Over();

		inline bool Is_Running() { return running;}

};

#endif

