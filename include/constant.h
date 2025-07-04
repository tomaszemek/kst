// project: Communication protocol, v. 1.0
// module: constant.h
// description: - definitions of constants used in the program
//		  		- all constants listed in a single header file
//				- sorted by alphabetical order of header files 
//				    in which which the constants were originally defined
//				- advantage of putting it all into one header:
//				    ability to use a constant in more modules without need 
//				    to include declarations of many other classes
// (c) Tomas Zemek, 2003, 2025

#ifndef _Constants_H
#define _Constants_H

// system headers:
	// none used

// project headers:
	#include "types.h"

// from app.h:
	// prioritization buffer size:
	constant _App_Priority_Buffer_Size = 32;

	// number of command line arguments for each of the program modes:
	constant _Argc_Passive = 2;
	constant _Argc_Active = 3;

	// position of command line arguments representing station ID:
	constant _Argv_Local_ID = 1;
	constant _Argv_Remote_ID = 2;

// from commprotocol.h:
	// upper limit for station ID (lower limit is 1)
	constant _Max_Station_ID = 99;

	// maximum count of connection establish retry attempts (single attempt duration timeout = 550 ms)
	constant _Max_Connect_Retries = 16;

	// prioritization buffer size:
	constant _Commprotocol_Priority_Buffer_Size = 100;

	// frame monitor buffer; maximum size reserved for single frame
	constant _Frame_Monitor_Buffer_Size = 80;	// max screen size

// from desktop.h:
	// screen (background) colour during
	constant _Init_Desktop = 17;	// during program runtime
	constant _Exit_Desktop = 15;	// after program finished

	// maximum count of simultaneously opened windows
	constant _Max_Windows_Opened = 100;

	// errors during work with windows:
	const int _No_Wnd = -1;	// no window reference

// from fcs.h:
	// count of table items:
	constant _FCS_Table_Size = 256;

	// FCS generating polynom:
	const word _FCS_Polynom = 0x1021;

// from frame.h:
	// constants determining frame type
	const byte ENQ = 0x05;	// Connection establish request
	const byte ACK = 0x06;	// a) Connection establish acknowledgement
							// b) Frame received correctly
							// c) Indication of readiness
	const byte NAK = 0x21;	// Frame received incorrectly
	const byte EOT = 0x04;	// Connection termination request
	const byte DAT = 0x07;	// Data transfer frame

	// size of data part of a frame
	constant _Max_Data_Size = 256;

	// maximum length of the buffer to store a frame (?)
	constant _Max_Buffer_Size = 546;

// from ipx.h:
	// socket number used for the program
	const word _KST_Socket = 0x5555;

	// receiving buffer size
	// count of received and not yet processed packets
	constant _Recv_Buffer_Size = 128;

	// IPX driver constants
	constant _IPX_Packet_Header_Size = 30;
	constant _IPX_Packet_Data_Size = 546;
	constant _IPX_ECB_Length = 42;
	constant _IPX_Interrupt_Number = 0x7A;		// Novell doest't recommend using,
											// but the 0x2F doesn't work

	// constant of the same meaning as the _IPX_Packet_Data_Length,
	// but with the purpose of printing or copying only the actually used data,
	// considering that we work with like 10 bytes, which are being put
	// into the data of IPX packet, it won't be necessary to refer to as many 
	// bytes as the _IPX_Packet_Data_Length does
	constant _KST_Packet_Data_Size_Modified = 40;

// from keybuf.h:
	// maximum number of simultaneously stored characters / keys:
	constant _KeyBuffer_Size = 16384; // 16 kB

// from log.h:
	// global logging switch:
	#define _Log_Enabled

	// logging types for fine tuning and help with debugging
	#define _Log_Type_Error	// error messages
	#define _Log_Type_Info  // informational messages, not state machine related, normal state but just printing out relevant info
	#define _Log_Type_State	// state change messages (to track state machine progress and state transitions)
	#define _Log_Type_Trace	// detailed flow messages, finest granularity
	#define _Log_Type_Debug // specific debug mask, only used on places of special interest
	#define _Log_Type_Dump 	// dump objects to log to see their actual values

	// buffer size to store all actual parameters to the call of C_Log::Log( ... )
	constant _Log_Buffer_Size = 1024;

// from sender.h:
	// sending window size - number of frames
	// can't change, value given by task specification
	constant _SendingFrameWindow_Size = 16;

// from window.h:
	// display dimensions (text mode)
	constant _Max_X = 80;
	constant _Max_Y = 25;

	// maximum length of window title:
	constant _Title_Length = _Max_X;

	// type of window frame:
	constant _Single_Border = 1;		// inactive window
	constant _Double_Border = 2;		// active window

	// background colour of a window area:
	constant _White_Text_Black_Background = 15;

	// maximum length of string to print at once:
	constant _Max_String_Length = 70;

// constants for priorities:
	// used in C_App::Message_Loop() and C_Commprotocol::Process_Event()
	enum E_Priority {

		priority_1	= 1,
		priority_2	= 2,
		priority_3	= 3,
		priority_4	= 4,
		priority_5	= 5,
		priority_6	= 6,
		priority_7	= 7,
		priority_8	= 8

	};

#endif