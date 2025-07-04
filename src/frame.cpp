// project: Communication protocol, v. 1.0
// module: frame.cpp
// description: C_Frame class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	#include <string.h>
	#include <alloc.h>

// project headers:
	#include "frame.h"

// methods of 'class' S_Frame:
S_Frame::S_Frame() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::S_Frame() - instance construction started : ");
	#endif

	m_header = 0;
	m_Ns = m_Nr = 0;
	m_sender = m_receiver = 0;

	m_size = 0;
	memset(m_data,'\0',_Max_Data_Size);

	m_FCS = 0;

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::S_Frame() - instance construction completed");
	#endif

	return;
}

S_Frame::S_Frame(const S_Frame& src_frame) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::S_Frame(const S_Frame&) - copy constructor, instance construction started : ");
	g_log->Log("\n S_Frame::S_Frame(const S_Frame&) - dump : ");
	#endif

	#ifdef _Log_Type_Dump
	src_frame.Dump();
	#endif

	m_header = src_frame.m_header;
	m_Ns = src_frame.m_Ns;
	m_Nr = src_frame.m_Nr;
	m_sender = src_frame.m_sender;
	m_receiver = src_frame.m_receiver;

	m_size = src_frame.m_size;
	if(m_size > 0)

		if( memcpy(m_data, src_frame.m_data, m_size) == NULL) g_error->Error("S_Frame::S_Frame(const S_Frame&) - failed to copy\n");

	m_FCS = src_frame.m_FCS;

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::~S_Frame(const S_Frame&) - instance construction completed");
	#endif

	return;
}

S_Frame::~S_Frame() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::~S_Frame() - instance destruction started : ");
	#endif

	m_header = 0;
	m_Ns = m_Nr = 0;
	m_sender = m_receiver = 0;

	m_size = 0;
	memset(m_data,'\0',_Max_Data_Size);

	m_FCS = 0;

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::~S_Frame() - instance destruction completed");
	#endif

	return;
}

S_Frame& S_Frame::operator=(const S_Frame& src_frame) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::operator=() - assignment started : ");
	#endif
	#ifdef _Log_Type_Dump
	g_log->Log("\n S_Frame::operator=() - dumping source object : ");
	src_frame.Dump();
	#endif

	if(&src_frame != this) {

		m_header = src_frame.m_header;
		m_Ns = src_frame.m_Ns;
		m_Nr = src_frame.m_Nr;
		m_sender = src_frame.m_sender;
		m_receiver = src_frame.m_receiver;

		m_size = src_frame.m_size;
		if(m_size > 0)

			if( memcpy(m_data, src_frame.m_data, m_size) == NULL) g_error->Error("S_Frame::operator=() - failed to copy\n");

		m_FCS = src_frame.m_FCS;
	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n S_Frame::operator=() - assignment completed");
	#endif

	return *this;
}

void S_Frame::Dump() const {

	#ifdef _Log_Type_Dump

	g_log->Log("\n S_Frame::Dump() - dumping current frame object:");
	g_log->Log("\n header = %d, Ns = %d, Nr = %d, sender = %d, receiver = %d", m_header, m_Ns, m_Nr, m_sender, m_receiver);
	g_log->Log("\n size = %d, data = '%s', FCS = %u", m_size, m_data, m_FCS);

	#endif

	return;
}

// Methods of class C_Frame
C_Frame::C_Frame() : m_frame() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::C_Frame() - instance construction started : ");
	#endif

	m_frame_buffer = new byte[_IPX_Packet_Data_Size];
	m_frame_buffer_size = _IPX_Packet_Data_Size;

	Create();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::C_Frame() - instance construction completed");
	#endif

	return;

}

C_Frame::C_Frame(const C_Frame& src_frame) : m_frame(src_frame.m_frame) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::C_Frame(const C_Frame&) - copy constructor, instance construction started : ");
	#endif
	#ifdef _Log_Type_Dump
	g_log->Log("\n C_Frame::C_Frame(const C_Frame&) - dumping source object : ");
	src_frame.Dump();
	#endif

	m_frame_buffer = new byte[_IPX_Packet_Data_Size];
	m_frame_buffer_size = _IPX_Packet_Data_Size;

	Create();

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::~C_Frame(const C_Frame&) - instance construction completed");
	#endif

	return;
}

C_Frame::~C_Frame() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::~C_Frame() - instance destruction started : ");
	#endif

	if(m_frame_buffer != NULL) delete [] m_frame_buffer;

	m_frame_buffer = NULL;
	m_frame_buffer_size = 0;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::~C_Frame() - instance destruction completed");
	#endif

	return;
}

// construct frame buffer from frame member items
void C_Frame::Create() {

	// allocate space (in bytes) (8 + m_size)
	m_frame_buffer_size = m_frame.m_size + 8;

	m_frame_buffer[0] = m_frame.m_header;
	m_frame_buffer[1] = m_frame.m_Ns;
	m_frame_buffer[2] = m_frame.m_Nr;
	m_frame_buffer[3] = m_frame.m_sender;
	m_frame_buffer[4] = m_frame.m_receiver;
	m_frame_buffer[5] = m_frame.m_size;

	// copy data there:
	// only when pointer to the source data doesn't point to NULL
	// !!! This could collide with Pascal implementation !!!
	if(m_frame.m_size > 0)

		for(int i = 0;i<m_frame.m_size;i++) m_frame_buffer[(6 + i)] = m_frame.m_data[i];

	// Intel uses 'Little Endian' format: (lower byte = lower address)

	m_frame_buffer[(6 + m_frame.m_size)] = (byte) m_frame.m_FCS;
	m_frame_buffer[(7 + m_frame.m_size)] = (byte) (m_frame.m_FCS >> 8);

	return;
}

// parse buffer and store individual member items
void C_Frame::Parse() {

	if(m_frame_buffer == NULL) g_error->Error("C_Frame::Parse() - 'm_frame_buffer' == NULL");

	m_frame.m_header = m_frame_buffer[0];
	m_frame.m_Ns = m_frame_buffer[1];
	m_frame.m_Nr = m_frame_buffer[2];
	m_frame.m_sender = m_frame_buffer[3];
	m_frame.m_receiver = m_frame_buffer[4];
	m_frame.m_size = m_frame_buffer[5];

	// update size:
	m_frame_buffer_size = 8 + m_frame.m_size;

	// copy data:
	// only copy when there is something present
	// !!! This could collide with Pascal implementation !!!
	if(m_frame.m_size > 0) {

		for(int i = 0;i<m_frame.m_size;i++) m_frame.m_data[i] = m_frame_buffer[(6 + i)];

	}

	// it's not a bad idea to add end of string character ('\0') after the data:
	m_frame.m_data[m_frame.m_size] = '\0';

	// verify that it works with Pascal implementation !!!
	m_frame.m_FCS = m_frame_buffer[(7 + m_frame.m_size)];
	m_frame.m_FCS <<= 8;
	m_frame.m_FCS += m_frame_buffer[(6 + m_frame.m_size)];

	return;
}

void C_Frame::Reset_Data() {

	m_frame.m_size = 0;
	memset(m_frame.m_data,'\0',_Max_Data_Size);

	return;
}

void C_Frame::Reset() {

	m_frame.m_header = 0;
	m_frame.m_Ns = 0;
	m_frame.m_Nr = 0;
	m_frame.m_sender = 0;
	m_frame.m_receiver = 0;

	Reset_Data();

	m_frame.m_FCS = 0;

	Create();

	return;
}

void C_Frame::Set(byte *src_buffer) {

	// copy data
	if( memcpy(m_frame_buffer, src_buffer, _KST_Packet_Data_Size_Modified) == NULL)

		g_error->Error("C_Frame::Set() - 'm_frame_buffer': failed to copy\n");

	// parse received buffer
	Parse();

	return;
}

void C_Frame::Set(const byte header) {

	m_frame.m_header = header;
	m_frame.m_Ns = 0;
	m_frame.m_Nr = 0;
	m_frame.m_sender = g_local_id;
	m_frame.m_receiver = g_remote_id;

	Reset_Data();

	m_frame.m_FCS = 0;

	Create();

	return;
}

void C_Frame::Set_DAT(uint character) {

	m_frame.m_header = DAT;
	m_frame.m_sender = g_local_id;
	m_frame.m_receiver = g_remote_id;

	Reset_Data();

	m_frame.m_size = 1;
	m_frame.m_data[0] = (byte) character;

	m_frame.m_FCS = 0;

	Create();

	return;
}

void C_Frame::Set_N(byte Ns, byte Nr) {

	m_frame.m_Ns = Ns;
	m_frame.m_Nr = Nr;

	Create();

	return;
}

void C_Frame::Set_FCS(word FCS) {

	m_frame.m_FCS = FCS;

	Create();

	return;
}

byte *C_Frame::Get_Frame_Buffer() {

	if(m_frame_buffer == NULL) Create();

	return m_frame_buffer;
}

int C_Frame::Get_Frame_Buffer_Size() {

	return m_frame_buffer_size;
}

byte C_Frame::Get_Character() {

	byte retval;

	if(m_frame.m_size == 1)

		retval = m_frame.m_data[0];

	else {

		#ifdef _Log_Type_Error
		g_log->Log("\n C_Frame::Get_Character() - wrong data format in the frame");
		#endif
		#ifdef _Log_Type_Dump
		g_log->Log("\n C_Frame::Get_Character() - dumping current frame");
		Dump();
		#endif

		retval = 0;
	}

	return retval;
}

C_Frame& C_Frame::operator=(const C_Frame& src_frame) {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::operator=() - assignment started : ");
	#endif
	#ifdef _Log_Type_Dump
	g_log->Log("\n C_Frame::operator=() - dumping source object : ");
	src_frame.Dump();
	#endif

	if(&src_frame != this) {

		m_frame = src_frame.m_frame;

		Create();
	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_Frame::operator=() - assignment completed");
	#endif

	return *this;
}

void C_Frame::Dump() const {

	#ifdef _Log_Type_Dump
	g_log->Log("\n C_Frame::Dump() - dumping current frame:");
	g_log->Log("\n header = %d, Ns = %d, Nr = %d, sender = %d, receiver = %d", m_frame.m_header, m_frame.m_Ns, m_frame.m_Nr, m_frame.m_sender, m_frame.m_receiver);
	g_log->Log("\n size = %d, data = '%s', FCS = %u", m_frame.m_size, m_frame.m_data, m_frame.m_FCS);
	g_log->Log("\n m_frame_buffer_size = %d",m_frame_buffer_size);

	g_log->Log("\n m_frame_buffer: ");
	for(int i=0;i<m_frame_buffer_size;i++) g_log->Log("%u, ",m_frame_buffer[i]);
	#endif

	return;

}

void C_Frame::Dump(byte *str) {

	sprintf(str,"'%d'   '%d' '%d'   '%d' '%d'   '%d' '%s'   '%u'", m_frame.m_header, m_frame.m_Ns, m_frame.m_Nr, m_frame.m_sender, m_frame.m_receiver, m_frame.m_size, m_frame.m_data, m_frame.m_FCS);

	return;
}