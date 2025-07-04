// project: Communication protocol, v. 1.0
// module: frame.h
// description: C_Frame class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _Frame_H
#define _Frame_H

// project headers:
	#include "types.h"
	#include "globvars.h"
	#include "constant.h"

// structure of a frame in the context 
//   of the KST application use case
// details in commprot.h
struct S_Frame {

	public:

		// byte = unsigned char
		// word = unsigned short int
		byte m_header;
		byte m_Ns;
		byte m_Nr;
		byte m_sender;
		byte m_receiver;
		byte m_size;
		byte m_data[_Max_Data_Size];
		word m_FCS;

	// basic operations:
		S_Frame();
		S_Frame(const S_Frame& src_frame);
		~S_Frame();
		S_Frame& operator=(const S_Frame& src_frame);

		void Dump() const;

};

// encapsulation of a frame structure into a class
class C_Frame {

	private:
	// for the purpose of KST point of view - frame accessible per items
		S_Frame m_frame;

	// for the purpose of IPX I/O - frame as array of bytes
		int m_frame_buffer_size;
		byte *m_frame_buffer;

	// conversion methods: IPX <-> KST format
		// build a frame from items
		void Create();

		// populate items from a frame
		void Parse();

	// reset everything:
		void Reset_Data();

	public:

	// constructors, destructor:
		C_Frame();
		C_Frame(const C_Frame& frame);	// just for check, only logs its call

		~C_Frame();

	// setters:
		// zero everything:
		void Reset();

		// set by using received data in a form of a buffer (array with size)
		void Set(byte *src_buffer);

		// build a frame as such:
		// - header is defined by a parameter
		// - Nr, Ns, data, FCS = 0
		// - station ID's are read from global variables
		void Set(const byte header);

		// create DAT frame:
		// sets everything except for frame numbering and FCS
		void Set_DAT(uint character);

		// sets frame numbering:
		void Set_N(byte Ns, byte Nr);

		// sets FCS
		void Set_FCS(word FCS);

	// getters:
		// returns a character;
		// logs error if the frame contains string instead
		byte Get_Character();

		// vraci ukazatel na pocatek (sestaveneho) ramce
		// returns pointer to the start of the (built) frame
		byte *Get_Frame_Buffer();

		// returns size of the (built) frame
		int Get_Frame_Buffer_Size();

		inline byte Get_Header()	{ return m_frame.m_header;}
		inline byte Get_Ns() 		{ return m_frame.m_Ns;}
		inline byte Get_Nr()		{ return m_frame.m_Nr;}
		inline byte Get_Sender()	{ return m_frame.m_sender;}
		inline byte Get_Receiver()		{ return m_frame.m_receiver;}
		inline byte Get_Size()	{ return m_frame.m_size;}
		inline word Get_FCS()	{ return m_frame.m_FCS;}

	// overloaded assignment operator:
		C_Frame& operator=(const C_Frame& src_frame);

	// dump frame to log:
		void Dump() const;

	// dump frame to a string:
		void Dump(byte *str);

};

#endif