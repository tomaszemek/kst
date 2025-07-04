// project: Communication protocol, v. 1.0
// module: window.h
// description: C_Window class declaration
// (c) Tomas Zemek, 2025

#ifndef _Window_Constants_H
#define _Window_Constants_H

// system headers:
	// none used

// project headers:
	#include "types.h"

// window types:
    enum Window_Type {

        Generic_Window = 1,     // Generic window with default behaviour of C_Window
        Status_Window = 2,      // Output only with line numbering, no input allowed
        Input_Window = 3,       // Allows input
        Output_Window = 4,      // Output only without line numbering, no input allowed
        Menu_Window = 5,        // Shows menu and allows moving up and down with highlighting current row
        Scrollback_Window = 6   // TODO: add history buffer and allow scrollback

    };

// types relevant for C_Window class:
    // constants that need to match a single character (single byte)
    typedef const unsigned char  constant_character;

// constants relevant for C_Window class:
    // window border characters mapping:

    // assumes encoding DOS CP437
    // the extended ASCII table has characters for table frames

    // table characters - frame, single border:

    // 179 = Box Drawings Light Vertical (left and right sides)
    // 196 = Box Drawings Light Horizontal (top and bottom sides)
    // 218 = Box Drawings Light Down And Right (top left corner)
    // 191 = Box Drawings Light Down And Left (top right corner)
    // 192 = Box Drawings Light Up And Right (bottom left corner)
    // 217 = Box Drawings Light Up And Left (bottom right corner)
    constant_character _Border_Single_Vertical_Side = 179;
    constant_character _Border_Single_Horizontal_Side = 196;
    constant_character _Border_Single_Top_Left_Corner = 218;
    constant_character _Border_Single_Top_Right_Corner = 191;
    constant_character _Border_Single_Bottom_Left_Corner = 192;
    constant_character _Border_Single_Bottom_Right_Corner = 217;
    
    // 186 = Box Drawings Double Vertical (left and right sides)
    // 205 = Box Drawings Double Horizontal (top and bottom sides)
    // 201 = Box Drawings Double Down And Right (top left corner)
    // 187 = Box Drawings Double Down And Left (top right corner)
    // 200 = Box Drawings Double Up And Right (bottom left corner)
    // 188 = Box Drawings Double Up And Left (bottom right corner)
    constant_character _Border_Double_Vertical_Side = 186;
    constant_character _Border_Double_Horizontal_Side = 205;
    constant_character _Border_Double_Top_Left_Corner = 201;
    constant_character _Border_Double_Top_Right_Corner = 187;
    constant_character _Border_Double_Bottom_Left_Corner = 200;
    constant_character _Border_Double_Bottom_Right_Corner = 188;


#endif
