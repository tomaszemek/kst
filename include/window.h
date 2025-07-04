// project: Communication protocol, v. 1.0
// module: window.h
// description: C_Window class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Window_H
#define _Window_H

// system headers:
	#include <conio.h>

// project headers:
	#include "types.h"
	#include "event.h"
	#include "constant.h"
	#include "winconst.h"
	#include "error.h"
	#include "log.h"

// global variables:
	import C_Error	*g_error;
	import C_Log	*g_log;

// own data types:
// represents one character on screen
struct C_Point {
	byte chr;	// character code
	byte atr;   // attribute (color foreground and background)
};

// C_Window = general window
// encapsulates window implementation
class C_Window {

	protected:
		// screen dimensions - due to mapping to VRAM
		int m_maxx, m_maxy;

		// window title - being shown
		char *m_title;

		// dimensions - position:
		int m_top_left_x, m_top_left_y;
		int m_width, m_height;

		// edges - position:
		int m_edge_left_x, m_edge_left_y, m_edge_right_x, m_edge_right_y;

		// inner area usable for text (to be able to place cursor within this area)
		int m_textarea_left_x, m_textarea_left_y, m_textarea_right_x, m_textarea_right_y;

		// current cursor position
		int m_cursor_x, m_cursor_y;

		// memory for contents - just the current contents, no history
		// same like the DOS prompt - all what had scrolled up is gone
		// mapped directly to the video memory in text mode at respective place
		// word = unsigned short int, lower byte = character code, upper byte = attribute
		// attribute: lower 4 bits = foreground color, upper 4 bits = background color
		// to access attributes - macro, function, ... - TODO

		// two buffers, one for data, the other as pointer to "my" place in VRAM

		// data buffer [height][width]
		C_Point **m_buffer;	// allocation of required memory - in constructor

		// pointer to the screen base in VRAM
		word m_screen_base;	// constant value

		// pointer to "my" area in VRAM
		C_Point **m_wnd_screen;

		// private usable methods:
		// - local method to check reaching windows edges when typing,
		//   in order to keep cursor in designated area
		// increments / decrements cursor one position further / back,
		// aware of typing area edges
		void Cursor_Decrement();
		void Cursor_Increment(bool newline = false);

		// scroll whole contents line up
		void Scroll_Up();

		// dump to log:
		void Dump();


	public:

		C_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		virtual ~C_Window();

		// return own title:
		inline char *Get_Title() { return m_title;}

		// show yourself
		// buffer contents will be displayed at the coordinates 
		// specified by members bdrxy
		void Show();

		// clear the contents (scroll up the whole window height)
		void Clear();

		// activate yourself = change edge (frame, borders) style, show contents and enable cursor
		inline void Activate()
		{
			Change_Frame_Style(_Double_Border);
			Show();
			Cursor_Enable();
			return;
		}

		// deactivate yourself: return single border style show yourself and disable cursor
		inline void Deactivate()
		{
			Cursor_Disable();
			Change_Frame_Style(_Single_Border);
			Show();
			return;
		}

		// show / hide cursor at your current position
		inline void Cursor_Enable()
		{
			gotoxy(m_top_left_x+m_cursor_x,m_top_left_y+m_cursor_y);
			_setcursortype(_NORMALCURSOR);
			return;
		}

		inline void Cursor_Disable()
		{
			_setcursortype(_NOCURSOR);
			return;
		}

		// change the border style - single or double line
		void Change_Frame_Style(int new_border);

		// print a character and increment cursor one step further
		void Print_Char(int c);

		// print a string and scroll up
		virtual void Print_String(const char *str);

};

class C_Status_Window : public C_Window {

	protected:

		bool line_numbering;

		int current_line_number;
	
	public:

		C_Status_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		~C_Status_Window();

		// print string with line numbering:
		void Print_String(const char *str);


};

class C_Input_Window : public C_Window {

		
	public:

		C_Input_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		~C_Input_Window();

};

class C_Output_Window : public C_Window {

	public:

		C_Output_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		~C_Output_Window();

};

class C_Menu_Window : public C_Window {

	public:

		C_Menu_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		~C_Menu_Window();

};

class C_Scrollback_Window : public C_Window {

	public:

		C_Scrollback_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y);
		~C_Scrollback_Window();

};


#endif // _Window_H
