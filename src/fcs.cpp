// project: Communication protocol, v. 1.0
// module: fcs.cpp
// description: C_FCS class definition
// (c) Tomas Zemek, 2003, 2025

// system headers:
	// not used

// project headers:
	#include "fcs.h"

// C_FCS class methods:
C_FCS::C_FCS() {

	// helper variables:
	word i,j;

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_FCS::C_FCS() - instance construction started : ");
	#endif

	// cycle to generate all combinations for the table:
	for(i=0;i<_FCS_Table_Size;i++) {

		m_fcs = (i << 8);

		// calculate FCS for individual combinations of the bits of the character
		for(j=8;j>0;j--) {

			if( m_fcs & 0x8000 )

				m_fcs = ((m_fcs << 1) ^ _FCS_Polynom);

			else

				m_fcs <<= 1;
		}

		m_FCS_table[i] = m_fcs;
	}

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_FCS::C_FCS() - instance construction completed");
	#endif

	return;
}

C_FCS::~C_FCS() {

	#ifdef _Log_Type_Trace
	g_log->Log("\n C_FCS::~C_FCS() - instance destruction completed");
	#endif

	return;
}

word C_FCS::Create_FCS(C_Frame *frame) {

	m_character = frame->Get_Character();

	m_fcs = 0;

	m_fcs = m_FCS_table[ (( m_fcs >> 8) ^ m_character) & 0xFF ] ^ (( m_fcs << 8 ) & 0xFF00 );

	frame->Set_FCS(m_fcs);

	return m_fcs;
}

bool C_FCS::Check_FCS(C_Frame *frame) {

	bool retval;

	m_character = frame->Get_Character();

	m_fcs = 0;

	m_fcs = m_FCS_table[ (( m_fcs >> 8) ^ m_character) & 0xFF ] ^ (( m_fcs << 8 ) & 0xFF00 );

	if(m_fcs == frame->Get_FCS()) retval = true;
	else retval = false;

	return retval;
}
