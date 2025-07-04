/*#############################################################################
# TYPES.H
#
# Date  : 14.02.1995 (Codor)
# Desc  : New types and constants needed throughout the project
#
# Modified : 24.4.2003, Tomas Zemek
#            27.6.2025, Tomas Zemek
#
#############################################################################*/

#ifndef _Types_H
#define _Types_H

typedef enum { false = 0, true } bool;

typedef unsigned char   byte;
typedef unsigned int    uint;
typedef unsigned short  word;
typedef unsigned long   dword;
typedef unsigned long   ulong;

typedef const unsigned int constant;

#define local	static	// local variable of the current module
#define global	/* */	// global variable - keyword used at the variable definition place
#define import  extern	// references a variable from elsewhere - keyword used 
//                         at all other place than the variable definition place
#define export	extern	// references a variable to be shared by others - keyword used 
//                         at the variable definition place

#endif

/*#############################################################################
# End of file TYPES.H
#############################################################################*/

