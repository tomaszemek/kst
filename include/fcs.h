// project: Communication protocol, v. 1.0
// module: fcs.h
// description: C_FCS class declaration
// (c) Tomas Zemek, 2003, 2025

#ifndef _FCS_H
#define _FCS_H

// project headers:
	#include "types.h"
	#include "frame.h"
	#include "globvars.h"
	#include "constant.h"

// working with FCS:
// implementation of FCS: semi-table algorithm based on the fact that 
// the received or sent octet generates constant remainder after division.
// This remainder is then added modulo 2 to the newly sent or received octet

// formula of the generating polynom: g(x) = x^16 + x^12 + x^5 + 1
class C_FCS {

	// table of FCS values:
		word m_FCS_table[_FCS_Table_Size];

	// helper variable for intermediate results during calculations
		word m_fcs;

	// evaluated character
		byte m_character;

	public:
		C_FCS();
		~C_FCS();

	// sign given frame before sending
	// returns calculated FCS; it also sets the 'FCS' attribute of the given frame
		word Create_FCS(C_Frame *frame);

	// check given frame after receiving
	// returns true = frame OK, false = FCS mismatch
		bool Check_FCS(C_Frame *frame);

};


#endif