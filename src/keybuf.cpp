// project: Communication protocol, v. 1.0
// module: keybuf.cpp
// description: C_KeyBuffer class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	// none used

// project headers:
	#include "keybuf.h"

C_KeyBuffer::C_KeyBuffer() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_KeyBuffer::C_KeyBuffer() - instance construction started : ");
	#endif

	// create buffer:
	buffer = new uint[_KeyBuffer_Size];

		if(buffer == NULL) g_error->Error("C_Keybuffer::C_KeyBuffer() - 'buffer': failed to allocate memory");

	// reset itself:
	Empty();
	
	#ifdef _Log_Type_Trace
	g_log->Log("\n C_KeyBuffer::C_KeyBuffer() - instance construction completed ");
	#endif

	return;
}

C_KeyBuffer::~C_KeyBuffer() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_KeyBuffer::~C_KeyBuffer() - instance destruction started : ");
	#endif

	delete [] buffer;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_KeyBuffer::~C_KeyBuffer() - instance destruction completed");
	#endif

	return;
}

uint C_KeyBuffer::Add_Key(uint key) {

	// test for full buffer:
	if(is_full == true) return (-1);

	buffer[write_index] = key;

	// increment write_index:
	if(write_index == (_KeyBuffer_Size-1)) write_index = 0;
	else write_index++;

	// if the write_index equals read_index, buffer got full:
	if(write_index == read_index) is_full = true;

    // not empty anymore:
	if(is_empty == true) is_empty = false;

	return 0;
}

uint C_KeyBuffer::Get_Key() {

	uint ret_key;

	// test for empty buffer:
	if(is_empty == true) return (-1);

	// read the current queue head key:
	ret_key = buffer[read_index];

	// increment read_index:
	if(read_index == (_KeyBuffer_Size-1)) read_index = 0;
	else read_index++;

	// if the indexes match, buffer is empty now:
	if(read_index == write_index) is_empty = true;

	// not full anymore:
	if(is_full == true) is_full = false;

	return ret_key;
}

bool C_KeyBuffer::Is_Key(uint key) {

	// test for empty buffer:
	if(is_empty == true) return false;

	// compare queue head key with the parameter:
	if(buffer[read_index] == key) return true;
	else return false;

}

void C_KeyBuffer::Empty() {

	// reset data:
	for(uint i = 0;i<_KeyBuffer_Size;i++) buffer[i] = 0;

	// reset indexes:
	read_index = write_index = 0;

	// reset state:
	is_empty = true;
	is_full = false;

	return;
}
