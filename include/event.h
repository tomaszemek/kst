// project: Communication protocol, v. 1.0
// module: event.h
// description: C_Event class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Event_H
#define _Event_H

// project headers:
	#include "types.h"

// key codes
// values in range 0 - 255 (matches the key usually)
enum {

	kb_extended		= 0,	// the extended key code would be in event.kbd.ext_code then
	kb_enter		= 13,
	kb_esc			= 27,
	kb_backspace	= 8,
	kb_tab	    	= 9

};

// message id
// values in range 300 - 400
enum {

	msg_nothing		= 300,
	msg_quit		= 301,
	msg_connect		= 303,
	msg_getkey		= 304,
	msg_getextkey	= 305,
	msg_sm_finished	= 306

};

// program modes: menu choices
// values in range 400 - 500
enum {

	mode_nothing		= 400,
	mode_active			= 401,
	mode_passive		= 402,
	mode_quit 			= 403,
	mode_change_local_ID	= 404,
	mode_frame_monitor	= 405

};

// communication protocol states
// inspired by finite state machine concept
// values in range 600 - 700
enum {

	// no connection
	state_600	= 600,	// no connection

	// initialization before connection establish attempt
	state_601	= 601,	// initialization before starting active connection establish attempt mode
	state_602	= 602,	// initialization before starting passive connection establish attempt mode
	state_603	= 603,	// inicialization before starting frame monitor mode

	// connection establish attempt
	state_610	= 610,	// active attempt (send ENQ, start timer_A, print a dot to status window)
	state_611	= 611,	// determine network state (active mode)
	state_612	= 612,	// determine network state (passive mode)
	state_613	= 613,	// ENQ from remote station received
	state_614	= 614,	// timer_A expired
	state_615	= 615,	// connection established, restart timer_B and timer_C

	// data transfer main event handling:
	state_620	= 620,	// connection established, main decision state
	state_621	= 621,	// frame sent, restart timer_B
	state_622	= 622,  // frame received, restart timer_C
	state_623	= 623,	// keyboard input, prepare frames in send window
	state_624	= 624,	// data waiting in send window, send
	state_625	= 625,	// timer_B expired - send heartbeat ACK(0,0)
	state_626	= 626,	// EOT received, send ACK
	state_627	= 627,	// ACK for received frame waiting to be sent

	// data transfer frame processing
	state_630	= 630,	// frame received
	state_631	= 631,	// DAT received, FCS check
	state_632	= 632,  // DAT received, FCS OK, order check
	state_633	= 633,	// DAT received, FCS OK, in order
	state_634	= 634,	// DAT received, FCS OK, out of order
	state_635	= 635,	// DAT received, FCS not OK
	state_636	= 636,	// ACK received
	state_637	= 637,	// NAK received

	// frame monitor mode:
	state_640	= 640,	// determine network state
	state_641	= 641,	// frame received, print contents to window

	// regular connection termination attempt
	state_650	= 650,	// connection established, ESC key pressed
	state_651	= 651,	// connection termination attempt in progress, determine network state

	// connection termination related states
	state_660	= 660,	// connection establish attempt, ESC key pressed
	state_661	= 661,	// connection establish attempt, timer_A expired
	state_662	= 662,	// connection established, ESC key pressed, ACK received
	state_663	= 663,	// connection established, ESC key pressed, ACK not received
	state_664	= 664,	// connection established, timer_C expired
	state_665	= 665,	// connection established, EOT received, ACK sent
	state_666	= 666,	// receiving window full
	state_667	= 667,	// sending window full

	// connection closed followup
	state_670	= 670,	// connection closed, show prompt for ESC key
	state_671	= 671,	// connection closed, wait for ESC key
	state_672	= 672,	// connection closed, cleanup

	// runtime errors and unexpected situations
	state_690	= 690	// unexpected situation (runtime error)

};

struct C_KbdEvent {

	uint code;  	// ASCII code of pressed key
	uint ext_code;	// extended key code (in case when the 'code' is 0)

};

struct C_StateMachineEvent {

	uint state;	// state of the communication protocol state machine

};

struct C_AppEvent {

	uint mode;
};

struct C_MsgEvent {

	uint code;
};

struct C_Event {

	C_KbdEvent kbd;
	C_StateMachineEvent sm;
	C_AppEvent app;
	C_MsgEvent msg;

	C_Event();
	~C_Event();

};

#endif // _Event_H
