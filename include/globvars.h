// project: Communication protocol, v. 1.0
// module: globvars.h
// description: Global variables declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Global_Variables_H
#define _Global_Variables_H

// project headers:
	#include "types.h"
	#include "error.h"
	#include "log.h"
	#include "desktop.h"

// global variables:

	// exception handling and logging
	// defined in the main program module, KST.cpp
	import C_Error	*g_error;
	import C_Log	*g_log;

	// windows - shared across the whole program
	// defined in the application module APP.cpp
	import C_Desktop *g_desktop;
	import int g_hwnd_local;
	import int g_hwnd_remote;
	import int g_hwnd_status;
	import int g_hwnd_frame_monitor;

	// IDs of stations involved in the current chat
	// defined in the application module APP.cpp
	import byte g_local_id;
	import byte g_remote_id;

#endif