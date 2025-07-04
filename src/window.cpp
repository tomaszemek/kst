// project: Communication protocol, v. 1.0
// module: window.cpp
// description: C_Window class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <string.h>
	#include <dos.h>
	#include <conio.h>

// project headers:
	#include "window.h"

// C_Window methods definition:
C_Window::C_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) {

	// helper variables
	int i, j;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Window::C_Window() - instance ('%s') construction started : ",title);
	#endif

	// screen:
	m_screen_base = screen_base;
	m_maxx = max_x;
	m_maxy = max_y;

	// TODO: check for invalid parameter values:

	// general parameters:
	m_top_left_x = corner_top_left_x;
	m_top_left_y = corner_top_left_y;

	m_width = width;
	m_height = height;

	// frame: edges
	m_edge_left_x = corner_top_left_x;
	m_edge_left_y = corner_top_left_y;
	m_edge_right_x = (corner_top_left_x+width-1);
	m_edge_right_y = (corner_top_left_y+height-1);

	// window: inner dimensions, usable for cursor placement
	m_textarea_left_x = corner_top_left_x+1;
	m_textarea_left_y = corner_top_left_y+1;
	m_textarea_right_x = m_edge_right_x-1;
	m_textarea_right_y = m_edge_right_y-1;

	// cursor - relative position against window placement
	//   (TODO - define this !!!!)
	// only desktop (window manager) uses absolute coordinates,
	//  as it manages the whole desktop
	// !!! CHANGED: - cursor coordinates are buffer indexes
	// needs to be in range 0 - (width - 1)
	m_cursor_x = 1;
	m_cursor_y = 1;

	// window title
	m_title = new char[_Title_Length];

		if(m_title == NULL) g_error->Error("C_Window::C_Window() - 'm_title': failed to allocate memory");

	strcpy(m_title,title);

	// data buffer allocation
	m_buffer = NULL;
	m_buffer = new C_Point *[m_height];		// allocate x pointers to rows (x = m_height)

		if(m_buffer == NULL) g_error->Error("C_Window::C_Window() - 'm_buffer': failed to allocate memory");

	for(i = 0;i<m_width;i++) {

		m_buffer[i] = NULL;
		m_buffer[i] = new C_Point[m_width];	// allocate columns for i-th row, column count = m_width

			if(m_buffer[i] == NULL) g_error->Error("C_Window::C_Window() - 'm_buffer[i]': failed to allocate memory");

	}

	// zero data buffer
	for(i = 0;i<m_height;i++) {

		for(j = 0;j<m_width;j++) {

			m_buffer[i][j].chr = 0;
			m_buffer[i][j].atr = _White_Text_Black_Background; // white text on black background

		}
	}

	// VRAM mapping (of my area)
	// solution: when mapping own "window" area of VRAM
	// each row should be placed individually
	// and then repaint the rows (copy rows)

	// construction of array of pointers to rows
	m_wnd_screen = NULL;
	m_wnd_screen = new C_Point *[m_height];		// allocate x pointers to rows (x = m_height)

		if(m_wnd_screen == NULL) g_error->Error("C_Window::C_Window() - 'm_wnd_screen': failed to allocate memory");

	// beware - allocation of rows and assignment of row pointers to them
	// might not be the best way
	// below the row pointers will be mapped directly to VRAM
	// for(i = 0;i<m_width;i++) wnd_screen[i] = new C_Point[m_width];	// allocate columns for i-th row, column count = m_width

	// better way:
	// assign each row pointer to specific address
	// algorithm:
	// - for each row pointer calculate the offset (which word from base is it, 
	//   based on position of the row start
	//   and create pointer and assign that to the row pointer
	for(i = 0;i<m_height;i++) {

		// calculate offset and set the i-th row pointer there
		m_wnd_screen[i] = (C_Point *) MK_FP(m_screen_base,((((m_top_left_y+i) - 1)*m_maxx + (m_top_left_x - 1))*2));

	}

	// initialize inner area - attributes
	// - kept at default

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Window::C_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Window::~C_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Window::~C_Window() - instance ('%s') destruction completed",m_title);
	#endif

}

void C_Window::Show() {

	// local variables
	// use: indexes, meaning screen position
	int i,j;

	// update VRAM with m_buffer contents

	// i ... rows
	for(i = 0;i<m_height;i++) {

		// i-th row, j ... columns
		// for all columns in the i-th row
		for(j = 0;j<m_width;j++) {

			m_wnd_screen[i][j].chr = m_buffer[i][j].chr;
			m_wnd_screen[i][j].atr = m_buffer[i][j].atr;

		}
	}

	#ifdef _Log_Type_Debug
	g_log->Log("\n C_Window::Show() '%s'",m_title);
	#endif

	return;
}

void C_Window::Clear() {

	// scroll up the whole height

	for(int i=0;i<m_height;i++) Scroll_Up();

	// reset cursor:

	m_cursor_x = m_cursor_y = 1;

	return;
}

void C_Window::Change_Frame_Style(int new_frame_style) {

	int i,j,m,n;	// indexes

	// redraw windows frame in m_buffer
	switch(new_frame_style) {

		case _Single_Border:

			// corners:
			m_buffer[0][0].chr = _Border_Single_Top_Left_Corner;
			m_buffer[0][(m_width-1)].chr = _Border_Single_Top_Right_Corner;
			m_buffer[(m_height-1)][0].chr = _Border_Single_Bottom_Left_Corner;
			m_buffer[(m_height-1)][(m_width-1)].chr = _Border_Single_Bottom_Right_Corner;

			// top and bottom side:
			for(j=1;j<(m_width-1);j++) {

				m_buffer[0][j].chr = _Border_Single_Horizontal_Side;
				m_buffer[(m_height-1)][j].chr = _Border_Single_Horizontal_Side;

			}

			// insert spaced title into the top side:
			j = m_width/2 - strlen(m_title)/2 - 1;

			m_buffer[0][j-1].chr = ' ';

			for(m=0;m<strlen(m_title);m++) m_buffer[0][j+m].chr = m_title[m];

			m_buffer[0][j+m].chr = ' ';

			// left and right side:
			for(i=1;i<(m_height-1);i++) {

				m_buffer[i][0].chr = _Border_Single_Vertical_Side;
				m_buffer[i][(m_width-1)].chr = _Border_Single_Vertical_Side;

			}

			break;

		case _Double_Border:

			// corners:
			m_buffer[0][0].chr = _Border_Double_Top_Left_Corner;
			m_buffer[0][(m_width-1)].chr = _Border_Double_Top_Right_Corner;
			m_buffer[(m_height-1)][0].chr = _Border_Double_Bottom_Left_Corner;
			m_buffer[(m_height-1)][(m_width-1)].chr = _Border_Double_Bottom_Right_Corner;

			// top and bottom side:
			for(j=1;j<(m_width-1);j++) {

				m_buffer[0][j].chr = _Border_Double_Horizontal_Side;
				m_buffer[(m_height-1)][j].chr = _Border_Double_Horizontal_Side;

			}

			// insert spaced title into the top side:
			j = m_width/2 - strlen(m_title)/2 - 1;

			m_buffer[0][j-1].chr = ' ';

			for(n=0;n<strlen(m_title);n++) m_buffer[0][j+n].chr = m_title[n];

			m_buffer[0][j+n].chr = ' ';

			// left and right side:
			for(i=1;i<(m_height-1);i++) {

				m_buffer[i][0].chr = _Border_Double_Vertical_Side;
				m_buffer[i][(m_width-1)].chr = _Border_Double_Vertical_Side;

			}

			break;

		default: break;
	}

	return;
}

void C_Window::Print_Char(int c) {

	// TODO: why is argument int while we can display only unsigned char?

	// place character into m_buffer at cursor coordinates
	switch(c) {

		case kb_enter:  Cursor_Increment(true);
						break;

		case kb_esc: break;

		case kb_tab: break;

		case kb_extended: break;

		case kb_backspace:	Cursor_Decrement();
							m_buffer[m_cursor_y][m_cursor_x].chr = 0;
							break;

		default:	m_buffer[m_cursor_y][m_cursor_x].chr = (unsigned char) c;
					Cursor_Increment();

					#ifdef _Log_Type_Debug
					g_log->Log("C_Window : '%d' (= '%c') printed to '%s'",c,(unsigned char)c,m_title);
					#endif

					break;

	}

	// apply changes to the screen
	Show();

	return;
}

void C_Window::Print_String(const char *str) {

	int i;	// index of a character in a string

	// store a string into m_buffer at cursor position
	for(i=0;str[i] != '\0';i++) {

		if(str[i] == '\n') Cursor_Increment(true);

		else {

			m_buffer[m_cursor_y][m_cursor_x].chr = (unsigned char) str[i];
			Cursor_Increment();
		}

	}

	// apply changes to the screen
	Show();

	return;
}

void C_Window::Dump() {

	// TODO: not implemented yet
	
	return;
}

void C_Window::Cursor_Decrement() {

	// current position at the row starting position?
	if(m_cursor_x == 1) {

		// go to the end of the previous row, unless this is the first row already
		if(m_cursor_y != 1) {

			m_cursor_y--;
			m_cursor_x = (m_width-2);

		}
		// otherwise there would need to be scroll down one row at this point,
		// but that is not implemented

	}
	else m_cursor_x--;

	gotoxy(m_top_left_x+m_cursor_x,m_top_left_y+m_cursor_y);

	return;
}

void C_Window::Cursor_Increment(bool newline) {

	switch(newline) {

		// new line true:
		case true:  // cursor at the last row?
					if(m_cursor_y == (m_height-2)) {

						Scroll_Up();
						m_cursor_x = 1;
						// m_cursor_y stays for good at the last row

					}
					// otherwise move down one row
					else {

						m_cursor_x = 1;
						m_cursor_y++;
					}
					break;

		// new line false:
		case false:	// cursor at the end of a row?
					if(m_cursor_x == (m_width-2)) {

						if(m_cursor_y == (m_height-2)) {

							// scroll up and new line
							Scroll_Up();
							m_cursor_x = 1;
						}
						else {

							// just a new line
							m_cursor_x = 1;
							m_cursor_y++;
						}

					}
					else m_cursor_x++;
					break;

	}

	gotoxy(m_top_left_x+m_cursor_x,m_top_left_y+m_cursor_y);

	return;
}

void C_Window::Scroll_Up() {

	int i,j;

	// copying
	// i ... rows
	for(i = 2;i<(m_height-1);i++) {

		// i-th row, j ... columns
		// for each column in the i-th row
		for(j = 1;j<(m_width-1);j++) {

			m_buffer[i-1][j].chr = m_buffer[i][j].chr;
			// m_buffer[i-1][j].atr = m_buffer[i][j].atr;

		}
	}

	// no contents on the last row:
	for(j=1;j<(m_width-1);j++) m_buffer[(m_height-2)][j].chr = 0;

	return;
}


// C_Status_Window methods definition:
C_Status_Window::C_Status_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) 
 : C_Window(corner_top_left_x,corner_top_left_y,width,height,title,screen_base,max_x,max_y) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Status_Window::C_Status_Window() - instance ('%s') construction started : ",title);
	#endif

	line_numbering = true;

	current_line_number = 0;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Status_Window::C_Status_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Status_Window::~C_Status_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Status_Window::~C_Status_Window() - instance ('%s') destruction completed",m_title);
	#endif

}

void C_Status_Window::Print_String(const char *str) {

	char *final_string = new char[_Max_String_Length];
	char *trimmed_argument = new char[_Max_String_Length];

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Status_Window::Print_String() - called, start of method");
	#endif

	// test length of argument and indicate error eventually:
	if (strlen(str) > _Max_String_Length) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Status_Window::Print_String() - given string too long : %d > _Max_String_Length (%d); will trim it before printing",strlen(str),_Max_String_Length);
		#endif

		// trim, copy one character less than maximum allowed, as end of string is not added:
		strncpy(trimmed_argument,str,(_Max_String_Length - 1));

		// append with end of string:
		trimmed_argument[(_Max_String_Length - 1)] = '\0';
	}
	else {

		trimmed_argument = (char *) str;

	}

	// update line number and combine line number with the string to print:
	if (line_numbering == true) {

		current_line_number++;

		sprintf(final_string,"%d: %s",current_line_number,trimmed_argument);
		
	}
	else {

		final_string = trimmed_argument;

	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Status_Window::Print_String() - end of method");
	#endif

	C_Window::Print_String(final_string);

	C_Window::Print_Char(kb_enter);

}


// C_Input_Window methods definition:
C_Input_Window::C_Input_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) 
 : C_Window(corner_top_left_x,corner_top_left_y,width,height,title,screen_base,max_x,max_y) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Input_Window::C_Input_Window() - instance ('%s') construction started : ",title);
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Input_Window::C_Input_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Input_Window::~C_Input_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Input_Window::~C_Input_Window() - instance ('%s') destruction completed",m_title);
	#endif

}


// C_Output_Window methods definition:
C_Output_Window::C_Output_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) 
 : C_Window(corner_top_left_x,corner_top_left_y,width,height,title,screen_base,max_x,max_y) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Output_Window::C_Output_Window() - instance ('%s') construction started : ",title);
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Output_Window::C_Output_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Output_Window::~C_Output_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Output_Window::~C_Output_Window() - instance ('%s') destruction completed",m_title);
	#endif

}


// C_Menu_Window methods definition:
C_Menu_Window::C_Menu_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) 
 : C_Window(corner_top_left_x,corner_top_left_y,width,height,title,screen_base,max_x,max_y) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Menu_Window::C_Menu_Window() - instance ('%s') construction started : ",title);
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Menu_Window::C_Menu_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Menu_Window::~C_Menu_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Menu_Window::~C_Menu_Window() - instance ('%s') destruction completed",m_title);
	#endif

}


// C_Scrollback_Window methods definition:
C_Scrollback_Window::C_Scrollback_Window(int corner_top_left_x,int corner_top_left_y,int width,int height,const char *title,word screen_base,int max_x,int max_y) 
 : C_Window(corner_top_left_x,corner_top_left_y,width,height,title,screen_base,max_x,max_y) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Scrollback_Window::C_Scrollback_Window() - instance ('%s') construction started : ",title);
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Scrollback_Window::C_Scrollback_Window() - instance ('%s') construction completed",title);
	#endif

}

C_Scrollback_Window::~C_Scrollback_Window() {

	// TODO: add freeing the memory steps

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Scrollback_Window::~C_Scrollback_Window() - instance ('%s') destruction completed",m_title);
	#endif

}

