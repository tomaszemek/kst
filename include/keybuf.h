// project: Communication protocol, v. 1.0
// module: keybuf.h
// description: C_KeyBuffer class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Key_Buffer_H
#define _Key_Buffer_H

// project headers:
	#include "types.h"
	#include "globvars.h"
	#include "constant.h"

// cyclic buffer to store characters
class C_KeyBuffer {

		uint *buffer;
		uint read_index;
		uint write_index;

		bool is_empty;
		bool is_full;

	public:

		C_KeyBuffer();
		~C_KeyBuffer();

		inline bool Is_Empty() { return is_empty;}

		inline bool Is_Full() { return is_full;}

		// adds a character (key code) to the buffer
		// for correct operation it is recommended to perform full buffer test 
		//   before trying to add a character
		// returns -1 on failure (full buffer, can't add, key ignored)
		// returns 0 on success
		uint Add_Key(uint key);

		// returns waiting character on the head of the queue
		// for correct operation it is recommended to perform empty buffer test
		//   before trying to read a character
		// returns -1 on failure (buffer empty, nothing to return)
		// returns character (key code) otherwise
		uint Get_Key();

		// performs a check if the character (key code) on the queue head position
		// is the same as the one provided as parameter
		// the method won't affect state of the queue
		// the queue head remains queue head
		bool Is_Key(uint key);

		// empty the buffer
		void Empty();

};


#endif
