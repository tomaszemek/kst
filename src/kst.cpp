// project: Communication protocol, v. 1.0
// module: kst.cpp
// description: Main program module
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <stdlib.h>
	#include <stdio.h>

// project headers:
	#include "app.h"

// conditional include:
	#ifdef _Log_Enabled

		#include <alloc.h>

	#endif

// global variables definition:
	global C_Error	*g_error;
	global C_Log	*g_log;

int main(int argc, char *argv[])
{
	// local variables:
	int retval;
	C_App *KSTApp;

	// global variables initialization:
	g_error = new C_Error;

		if(g_error ==  NULL) {

			fprintf(stderr,"\n main(), 'g_error' : error during memory allocation, program will terminate\n");
			exit(-1);

		}

	#ifdef _Log_Enabled
	g_log = new C_Log;

		if(g_log == NULL) g_error->Error("main(), 'g_syslog' : error during memory allocation");

	g_log->Log("\n main() - free memory left : %lu B ",farcoreleft());
	#endif

	// create the application instance:
	KSTApp = new C_App;

		if(KSTApp == NULL) g_error->Error("main(), 'KSTApp' : error during memory allocation");

	// and run it:
	retval = KSTApp->Run(argc,argv);

	// graceful shutdown and cleanup:
	delete KSTApp;

	#ifdef _Log_Enabled
	g_log->Log("\n main() - process 'KSTApp' ended with return value '%d'",retval);

	g_log->Log("\n main() - free memory left : %lu B ",farcoreleft());
	#endif

	// global variables destruction and cleanup:
	delete g_log;
	delete g_error;

	return retval;
}