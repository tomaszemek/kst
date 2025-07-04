// project: Communication protocol, v. 1.0
// module: desktop.h
// description: C_Desktop class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Desktop_H
#define _Desktop_H

// system headers:
	// none used

// project headers:
	#include "window.h"
	#include "event.h"
	#include "constant.h"
	#include "error.h"
	#include "log.h"

// global variables:
	// in this module can't be the globvars.h included as it contains reference to C_Desktop
	// so declaring imported variables explicitly without including globvars.h:
	import C_Error	*g_error;
	import C_Log	*g_log;

// class C_Desktop represents the 'desktop' of the program, manages windows on the screen,
// takes care of their de/activation, sends them messages
class C_Desktop {

		// VRAM - base address
		// it is enough to initialize once after program startup
		// (it depends on type of the hardware and that won't change in runtime)
		word m_screen_base;

		// screen dimensions (width and height) are needed to know
		// they could change in runtime (really ???)
		// these are absolute numbers, not usable for cursor directly
		// i.e. for resolution 80x25 then the m_max_x = 80, m_max_y = 25
		int m_max_x, m_max_y;

		// screen resolution change in runtime is realized through
		// public methods in C_Desktop and C_Window, there the recalculation 
		// of the mapping pointers takes place

		// Access structure - array of pointers to windows
		// array index = wnd_ID;
		// support for z-index is not implemented
		// basic pointer to the access vector to windows
		C_Window **m_wndvect;
		
		// highest used index:
		int m_max_id;
		// currently active index (i.e. the window where output is redirected to
		// and where cursor is blinking)
		int m_cur_id;

		// empty desktop = no window
		bool m_empty;

		// start from the beginning of the vector and find first free spot
		// doesn't change the pointers (m_cur_id), works with MAX_WINDOWS
		// with full vector returns MAX_WINDOWS
		int Find_First_Free();

		// start from the m_cur_id towards the end of the vector, cyclic,
		// and find the nearest next (following) existing window
		// doesn't change the pointers (m_cur_id, m_max_id)
		int Find_Next_Wnd(int from_id);

		// find nearest previous existing windows, cyclic,
		// nemeni ukazatele (cur_id)
		// doesn't change the pointers (m_cur_id)
		int Find_Prev_Wnd(int from_id);

	public:
		C_Desktop();
		~C_Desktop();

		// clear screen
		// values for mode: _Init_Desktop, _Exit_Desktop
		void Clear(unsigned int mode);

		// repaints everything from scratch - desktop and all windows
		void Repaint();

		// create new window
		int Create_Window(Window_Type window_type,const char *title,int corner_top_left_x,int corner_top_left_y,int width,int height);

		// destruct a window
		void Destruct_Window(int wnd_id);

		// clear contents of current active window
		void Clear_Window(void);

		// clears given window
		void Clear_Window(int hwnd);

		// get ID of current active window
		int Get_Current_Window_ID();

		// set current active window based on given ID and highlight it
		void Set_Current(int wnd_id);

		// set next window (next following in the (cyclic) vector)
		void Set_Next_Current();

		// switch current (active) window from one to another
		void Switch_Windows(int old_wnd, int new_wnd);

		// print character into current active window
		void Print_Char(int c);

		// print string into current active window
		void Print_String(const char *str);

		// print character into given window
		void Print_Char_To_Window(int wnd_id,int c);

		// print string into given window
		void Print_String_To_Window(int wnd_id,const char *str);

		// respond to an event, handle a message
		void Process_Event(C_Event *event);

};

#endif // _Desktop_H