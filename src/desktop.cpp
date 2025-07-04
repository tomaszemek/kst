// project: Communication protocol, v. 1.0
// module: desktop.cpp
// description: C_Desktop class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <dos.h>
	#include <conio.h>

// project headers:
	#include "desktop.h"

// C_Desktop methods:
C_Desktop::C_Desktop() {

	int i;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::C_Desktop() - instance construction started : ");
	#endif

	// initialize VRAM - enough to perform once and then just distribute
	// determine VRAM base
	union REGS r;
	r.h.ah = 0x0F;
	int86(0x10, &r, &r);
	switch (r.h.al) {
		case 7:
			m_screen_base = (word) 0xB000;
			break;
		case 2:
		case 3:
			m_screen_base = (word) 0xB800;
			break;
	}

	// determine screen resolution
	// alternatively: fixed set screen resolution to mode 80x25
	// problem: need to switch to that mode; TODO
	textmode(C80);

	m_max_x = 80;
	m_max_y = 25;

	// initialize windows list
	// construction of the access vector
	m_wndvect = NULL;
	m_wndvect = new C_Window *[_Max_Windows_Opened];

		if(m_wndvect == NULL) g_error->Error("C_Desktop::C_Desktop() - 'm_wndvect': failed to allocate memory");

	// mark all items as invalid (no window there)
	for(i = 0;i < _Max_Windows_Opened;i++) m_wndvect[i] = NULL;

	m_max_id = 0;		// highest used element
	m_cur_id = 0;		// current element

	m_empty = true;		// empty desktop

	// clear screen
	Clear(_Init_Desktop);

	// TODO: Note: it would be better when constructor would read the initial attribute
	// (attribute is property of each character on screen defining foreground and background colors)
	// from the system and set that one back on exit (in destructor)

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::C_Desktop() - instance construction completed");
	#endif

}

C_Desktop::~C_Desktop() {

	int rmvd_id;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::~C_Desktop() - instance destruction started : ");
	#endif

	// clear screen
	Clear(_Exit_Desktop);

	// destruct opened hanging windows
	if(m_empty == false) {

		// walk the vector from start to end and destruct every hanging window
		for(rmvd_id = 0;rmvd_id <= _Max_Windows_Opened;rmvd_id++) {

			if(!(m_wndvect[rmvd_id] == NULL)) delete m_wndvect[rmvd_id];

		}
	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::~C_Desktop() - instance destruction completed");
	#endif

}

void C_Desktop::Clear(unsigned int mode) {

	// clear screen

	// algorithm: iterate the whole displayed area 
	// and set attribute to respective value
	int i,j;
	C_Point **ptr;

	ptr = new C_Point *[m_max_y];	// allocate x row pointers

	for(i = 0;i<m_max_y;i++) {

		// calculate offset, set the i-th row pointer there
		ptr[i] = (C_Point *) MK_FP(m_screen_base,(i * m_max_x * 2));

	}

	for(i = 0;i<m_max_y;i++) {

		// i-th row, j = columns
		// for all columns in i-th row
		for(j = 0;j<m_max_x;j++) {

			ptr[i][j].chr = 0;
			ptr[i][j].atr = (unsigned char) mode;

		}

	}

	return;
}

int C_Desktop::Create_Window(Window_Type window_type,const char *title,int corner_top_left_x,int corner_top_left_y,int width,int height) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Create_Window() - creating window for '%s' ...",title);
	#endif

	// leave old window, deactivate it, disable cursor
	
	if(m_empty == false) {	// = is there any window there?

		// leave the current window
		m_wndvect[m_cur_id]->Deactivate();

	}
	else m_empty = false;

	// create new one
	// find the lowest empty element

	m_cur_id = Find_First_Free();

	if(m_cur_id == _Max_Windows_Opened) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Create_Window() - too many opened windows, failed to create '%s'",title);
		#endif

		return (_No_Wnd);
	}

	// the m_cur_id holds the lowest empty position, can be used to create window
		
	// select appropriate window type:
	switch(window_type) {

		case Status_Window:

			m_wndvect[m_cur_id] = new C_Status_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);
			
			break;

		case Input_Window:

			m_wndvect[m_cur_id] = new C_Input_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);

			break;

		case Output_Window:

			m_wndvect[m_cur_id] = new C_Output_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);

			break;

		case Menu_Window:

			m_wndvect[m_cur_id] = new C_Menu_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);

			break;

		case Scrollback_Window:

			m_wndvect[m_cur_id] = new C_Scrollback_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);

			break;

		case Generic_Window:

		default:

			m_wndvect[m_cur_id] = new C_Window(corner_top_left_x,corner_top_left_y,width,height,title,m_screen_base,m_max_x,m_max_y);

			break;

	}
	
	m_wndvect[m_cur_id]->Activate();

	// update administration
	if(m_cur_id > m_max_id) m_max_id = m_cur_id;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Create_Window() - window created for '%s' wnd_id = %d",title,m_cur_id);
	#endif

	return m_cur_id;
}

void C_Desktop::Destruct_Window(int wnd_id) {

	// need to determine if the destructed is the current one or another one
	// a) destructing current one:
	// 		- find previous
	//		- destruct current one
	//		- switch current one to previous
	// b) destructing another one, not a current one:
	//		- keep pointer to current one
	//		- destroy given one
	//		- ... ?

	// in both cases need to update m_max_id !!!
	// one more problem: what if m_max_id == m_cur_id == 0 ???

	// if this gets commented, failure occurs
	// if this gets uncommented, failure won't occur but program won't work:
	// (even when noone notices)
	/*
	if(m_empty == true) {

		g_log->Log("\n C_Desktop::DestroyWnd() - attempted to destruct window in empty desktop");
		return;
	}
	*/

	// attempted to destruct nonexisting window:
	if(wnd_id == (_No_Wnd)) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Destruct_Window() - attempted to destruct nonexistent window !");
		#endif

		return;
	}
	else {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Destruct_Window() - destructing '%d'",wnd_id);
		#endif
	}

	// a) destructing current one:
	// - it is possible that current one is the last existing one
	// - but it is also possible that somewhere lower there is another existing one
	if(wnd_id == m_cur_id) {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Destruct_Window() - current == destructed");
		#endif

		m_max_id = Find_Prev_Wnd(m_cur_id);

		// test if that one is the last existing one
		if(m_cur_id == m_max_id) {

			#ifdef _Log_Type_Trace
			g_log->Log("\n C_Desktop::Destruct_Window() - destructing last existing window");
			#endif

			m_empty = true;	// now the desktop is empty
		}

		// destruct current one
		delete m_wndvect[m_cur_id];
		m_wndvect[m_cur_id] = NULL;

		// and switch to lower one still existing
		// ??? if they were equal, the current one now points to non existing one?
		// should the m_max_id be set to NULL (or better, _NoWnd) previously in the if ?
		m_cur_id = m_max_id;

		#ifdef _Log_Type_Dump
		g_log->Log("\n C_Desktop::Destruct_Window() - dump : m_cur_id = %d, m_max_id = %d, m_empty = %d", m_cur_id, m_max_id, (int)m_empty);
		#endif

		Repaint();
	}

	// b) destructing non current one
	// - means there is at least one another window
	else {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Destruct_Window() - current != destructed");
		#endif

		// destruct given window
		delete m_wndvect[wnd_id];
		m_wndvect[wnd_id] = NULL;

		// was that a m_max_id window? -> update m_max_id then
		if(wnd_id == m_max_id) {

			// find nearest lower one:
			m_max_id = Find_Prev_Wnd(m_max_id);

		}

		#ifdef _Log_Type_Dump
		g_log->Log("\n C_Desktop::Destruct_Window() - dump : m_cur_id = %d, m_max_id = %d, m_empty = %d", m_cur_id, m_max_id, (int)m_empty);
		#endif

		Repaint(); // here it almost works

	}

	return;
}

void C_Desktop::Clear_Window(void) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Clear_Window(void) - clearing current window contents");
	#endif

	if(m_wndvect[m_cur_id] == NULL)

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Clear_Window(void) - current window doesn't exist");
		#else
		;
		#endif

	else

		m_wndvect[m_cur_id]->Clear();

	return;
}

void C_Desktop::Clear_Window(int hwnd) {

	int old_cur = m_cur_id;

	Set_Current(hwnd);

	Clear_Window();

	Set_Current(old_cur);

	return;
}

void C_Desktop::Repaint() {

	int paint_id;

	// m_cur_id holds current window, that one has bo be painted at the top
	// rather store the value:
	// old_m_cur_id = m_cur_id;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Repaint() - repainting desktop ...");

	g_log->Log("\n C_Desktop::Repaint() - before calling Clear()");
	#endif

	// desktop
	Clear(_Init_Desktop);

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Repaint() - Clear() completed");
	#endif

	// if the desktop is not empty, repaint all windows too:
	if(m_empty == false) {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Repaint() - desktop not empty, there is content to repaint");
		#endif

		// all windows from 0 up to m_max_id:
		for(paint_id=0;paint_id<=m_max_id;paint_id++) {

			#ifdef _Log_Type_Dump
			g_log->Log("\n C_Desktop::Repaint() - dump : m_cur_id = %d, m_max_id = %d, paint_id = %d", m_cur_id, m_max_id, paint_id);
			#endif

			if(!(m_wndvect[paint_id] == NULL)) {		// if that windows exists

				// then repaint it, but only if (paint_id == 0) is false
				// because then the m_max_id window would be deactivated and repainted too then

				#ifdef _Log_Type_Trace
				g_log->Log("\n C_Desktop::Repaint() - window '%d' exists",paint_id);
				#endif

				// deactivate previous one first
				if(paint_id) m_wndvect[Find_Prev_Wnd(paint_id)]->Deactivate();

				// activate current one:
				m_wndvect[paint_id]->Activate();
			}
			else {

				#ifdef _Log_Type_Trace
				g_log->Log("\n C_Desktop::Repaint() - window '%d' doesn't exist",paint_id);
				#endif
			}

		}

		// put the current one on the top
		// the one that was repainted as last (and that was m_max_id) needs to be deactivated
		m_wndvect[m_max_id]->Deactivate();
		m_wndvect[m_cur_id]->Activate();

	}
	else {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Repaint() - desktop is empty, nothing to repaint");
		#endif

	}

	#ifdef _Log_Type_Dump
	g_log->Log("\n C_Desktop::Repaint() - dump : m_cur_id = %d, m_max_id = %d, m_empty = %d", m_cur_id, m_max_id, (int)m_empty);
	#endif

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Repaint() - desktop repainted, current window = '%d'",m_cur_id);
	#endif

	return;
}


int C_Desktop::Get_Current_Window_ID() {

	if(m_empty == false) return m_cur_id;
	else return (_No_Wnd);
}

void C_Desktop::Set_Current(int wnd_id) {

	if(m_wndvect[wnd_id] == NULL) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Set_Current() - requested window ('%d') doesn't exist",wnd_id);
		#endif

		return;
	}

	if(wnd_id != m_cur_id)

		Switch_Windows(m_cur_id,wnd_id);

	else

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Set_Current() - requested window ('%d') is already set current ('%d')",wnd_id,m_cur_id);
		#else
		;
		#endif

	return;
}

void C_Desktop::Set_Next_Current() {

	int next_id;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Set_Next_Current() - current window == '%d'",m_cur_id);
	#endif

	next_id = Find_Next_Wnd(m_cur_id);

	Switch_Windows(m_cur_id,next_id);

	return;
}

void C_Desktop::Switch_Windows(int old_wnd, int new_wnd) {

	// switches from one to another
	// the first one needs to be correctly deactivated, the other one needs to be activated
	// and info needs to be updated

	if(old_wnd == new_wnd) {

		#ifdef _Log_Type_Trace
		g_log->Log("\n C_Desktop::Switch_Windows() - requested to switch same windows (old_wnd = '%d', new_wnd = '%d')",old_wnd,new_wnd);
		#endif

		return;
	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Switch_Windows() - switching from '%d'('%s') to '%d'('%s')",old_wnd,m_wndvect[old_wnd]->Get_Title(),new_wnd,m_wndvect[new_wnd]->Get_Title());
	#endif

	m_wndvect[old_wnd]->Deactivate();

	m_wndvect[new_wnd]->Activate();

	m_cur_id = new_wnd;

	return;
}

void C_Desktop::Print_Char(int c) {

	m_wndvect[m_cur_id]->Print_Char(c);

	return;
}

void C_Desktop::Print_String(const char *str) {

	m_wndvect[m_cur_id]->Print_String(str);

	return;
}

void C_Desktop::Print_Char_To_Window(int wnd_id, int c) {

	// int old_m_cur_id = m_cur_id;	// store originally current window

	if((wnd_id == _No_Wnd) || (m_wndvect[wnd_id] == NULL)) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Print_Char_To_Window() - attempted to print to nonexistent window ('%d') !!!",wnd_id);
		#endif

		return;

	}

	// g_log->Log("C_Desktop : Print_Char_To_Window('%d','%c')",wnd_id,(unsigned char)c);

	/*
	// --- originally it was with switching:
	if(old_m_cur_id != wnd_id) Switch_Windows(old_m_cur_id,wnd_id);	// switch to the new one

	Print_Char(c);				// print the char there

	if(old_m_cur_id != wnd_id) Switch_Windows(wnd_id,old_m_cur_id);	// switch window back

	m_cur_id = old_m_cur_id;
	*/

	// now it's without switching:
	m_wndvect[wnd_id]->Print_Char(c);

	return;
}

void C_Desktop::Print_String_To_Window(int wnd_id, const char *str) {

	// int old_m_cur_id = m_cur_id;	// store originally current window

	if((wnd_id == _No_Wnd) || (m_wndvect[wnd_id] == NULL)) {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Desktop::Print_String_To_Window() - attempted to print to nonexistent window ('%d') !!!",wnd_id);
		#endif

		return;

	}

	/*
	// --- originally it was with switching:
	if(old_m_cur_id != wnd_id) Switch_Windows(old_m_cur_id,wnd_id);	// switch to the new one

	Print_String(str);				// print the string there

	if(old_m_cur_id != wnd_id) Switch_Windows(wnd_id,old_m_cur_id);	// switch window back

	m_cur_id = old_m_cur_id;
	*/


	// now it's without switching:
	m_wndvect[wnd_id]->Print_String(str);

	return;
}

void C_Desktop::Process_Event(C_Event *event) {

	// case a) - message from the keyboard, key pressed; 
	// - check if it's not a control character,
	//   if not, print it in current window	
	// case a) cancelled
	// case b) 
	// - catch only control keys
	// DOESN'T WORK !!!!! TODO - never called ????
	switch(event->msg.code) {

		case msg_getkey:

			switch(event->kbd.code) {

				case kb_tab:	// = windows switching key
								#ifdef _Log_Type_Trace
								g_log->Log("\n C_Desktop::Process_Event() - pressed 'Tab', current window is '%d'('%s')",m_cur_id,m_wndvect[m_cur_id]->Get_Title());
								#endif

								Set_Next_Current();

								break;

				// no other control keys implemented at the moment

				default:		// print character in current window
								// Print_Char((int)event.kbd.code); <- change in case b) (commented)
								break;

			}

			break;

		default:

			break;

	}

	return;
}

int C_Desktop::Find_First_Free() {

	int free_id;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_First_Free() - starting lookup ... ");
	#endif

	for(free_id=0;free_id<_Max_Windows_Opened;free_id++) {

		// check if this element empty
		// problem - comparing FAR pointers !!!
		// but possibly with NULL it might work?
		if(m_wndvect[free_id] == NULL) break;	// found

	}

	// TODO: feels like something is missing here
	// is it always gonna find a free one?

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_First_Free() - found '%d', current window is '%d'",free_id,m_cur_id);
	#endif

	return free_id;
}

int C_Desktop::Find_Next_Wnd(int from_id) {

	int next_id = (from_id+1);	// next after current one
	bool found = false;

	// starting with 'one after from_id'

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_Next_Wnd() - 'from_id' == '%d', current window is '%d', starting lookup ... ",from_id,m_cur_id);
	#endif

	while(found == false) {

		if(m_wndvect[next_id] == NULL) {

			// two options: overflown beyond the last element, or hit a gap
			if(next_id > m_max_id) next_id = 0;	// move to the start
			else next_id++;						// move further on
		}
		else found = true;	// found

	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_Next_Wnd() - found '%d', current window is '%d'",next_id,m_cur_id);
	#endif

	return next_id;
}

int C_Desktop::Find_Prev_Wnd(int from_id) {

	int prev_id;
	bool found = false;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_Prev_Wnd() - 'from_id' == '%d', current window is '%d', starting lookup ... ",from_id,m_cur_id);
	#endif

	// initialize pointers - test if we're at the vector start
	prev_id = ( (from_id == 0) ? m_max_id : (from_id-1) );

	while(found == false) {

		if(m_wndvect[prev_id] == NULL) {

			// hit a gap, step over it direction down
			prev_id = ( (prev_id == 0) ? m_max_id : (prev_id-1) );

		}
		else found = true;	// found

	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Desktop::Find_Prev_Wnd() - found '%d', current window is '%d'",prev_id,m_cur_id);
	#endif

	return prev_id;
}