Program description:

- KST stands for a study course 'Komunikacni Site a Techniky' (Communication Networks and Techniques)
- realization of a communication protocol at link layer; usage: simple chat point-to-point
- it doesn't make use of external libraries in order to practice own programming skill and learn new things
- platform: MS-DOS or Windows terminal window
- requires IPX protocol to work (as per requirements)
- most of the documentation is in the source code itself
- program was architectured and designed with modularity in mind:
	- kst.cpp - the main() function, creates global objects 'log' and 'error'	
	- app.h - application, main loop of receive and distribute 'messages' - implemented only key pressed 
	- types.h - most used types
	- constant.h - global header with all major constants
	- event.h - 'event' to carry a 'message' - App, Desktop and Commprot have a method Process_Event to handle that
	- kbd.h - deal with keyboard
	- desktop.h + window.h - an approach to manage UI in text mode
	- commprot.h - actual communication protocol, implements finite state machine
	- ipx.h - actual network client, uses IPX DOS driver
	- frame.h - a 'frame' of the communication protocol
	- the rest are various helper classes to implement the whole functionality effectively

- architecture and design concepts:
	- encapsulation and modularization
	- use named constants as much as possible
	- potential portability in mind (to IP protocol instead of IPX or to another windows implementation)
	- restricted by running under DOS, being single thread limited, still trying to operated as much real time as possible by using priority indicator array message loop based on polling event sources (keyboard, network, timer) instead of waiting for event by blocking I/O.
	- single point of switching state (the main control variable of the state machine) making it very easy to track the protocol changes.
	- defined set of states and state transitions, making it very easy to debug the protocol.
	- overloaded operators and copy constructors making working with frame objects more straightforward.
	- windows implementation designed with inheritance and polymorphism in mind. Different types of windows could act differently, yet all perform similar set of operations.
		for example, status window provides line counters by default, yet it's qua interface identical to others. Other windows types are prototyped but not implemented yet (menu with arrow keys support, scrollback window with history, ...)
	- once chat session is active, there are three windows that need to be tracked simultaneously and independently. Characters can only be put at the cursor position, so that one need to be tracked and switched between windows properly whenever an event concerning any of the windows occurs. Also backspace character may show up at any moment and window borders need to be respected as well.
	- using real time clock interrupt handler to implement timers.
	- the program size kept growing and exceeded 64 kB at some point, making it necessary to upgrade memory model to be able to address that address space. It was chosen to go with Large model, that enabled enough address space for code and also for data.
	- interfacing with the IPX driver requires ESR, interrupt handler to be called asynchronously when network data are available. Integration of ESR with the rest of the application required some black magic voodoo but seem to run smooth in the end. There are various calling modifiers used across the IPX module, that is indication to compiler to not perform optimizations or assume anything, as IPX data could be modified by the ESR at any time.
	- one way of handling exceptions and one way of managing logs and various output messages. Programmable logging mask (set in build time) determining how much output will be generated and put to logfile.


fcstab.txt : codes pro the FCS - specific checksum field used to sign data frames


Program was built with Borland C++ 3.1 for DOS compiler

Compiler settings:
- Code generation:
	model: large
	options:
	- treat enums as ints
	- duplicate strings merged
	- unsigned characters
	assume SS equals DS: default for memory model

- Advanced code generation:
	floating point: 80287/387
	instruction set: 80386
	options:
	- generate underbars
	- fast floating point
	- fast huge pointers

- Entry/Exit code:
	prolog/epilog code generation: DOS standard
	calling convention: C
	stack options: all disabled

- Optimizations options:
	optimizations: all enabled, except for:
	- Assume no pointer aliasing
	- Standard stack frame
	- Object data calling
	register variables: automatic
	common subexpressions: optimize globally
	optimize for: speed


During development and testing were all optimizations disabled.
Once the major issues were fixed the optimizations were turned on up to the state listed above.
The known issues listed below occured already during testing phase.
If the optimization option 'Assume no pointer aliasing' is enabled, the connection with IPX driver fails (return value in AL during driver initialization is 00).


Known issues:

- when operating under Windows 98 there is unknown error occuring causing the program to hang.
It won't occur in Windows 95. It occurs during key and press of a character key.
Remote peer detects connection disconnected (timer_C expired).
Tip to fix: write own keyboard interrupt handler 0x09.

- sometimes it occurs that in the 'Remote' window not all characters typed by the remote peer.
So far it's not known is the problem is in the sender or receiver.

- operator delete won't actually release the dynamically allocated memory, so the available memory in far heap slowly decreases (as seen in the logfile).
The reason to this behavior is unknown.


Notes:

- the program kst_fix was developed as an attempt to fix issues that occured after main program crashed badly

- if in methods of the C_IPX class (especially in the private ones) a combination of 
disable() and enable() occurs, then under Win98 the program likely hangs, under Win95 it works fine.

- the C_Error class: it simplifies exception handling by doing it at one place,
but there is no correct cleanup and graceful shutdown performed - no files are closed, memory freed, ...
This becomes a problem typically once the program hanged.

- it would be worth trying to optimize the array of priority indicators in app.cpp and commprot.cpp,
which are used in the Message_Loop and Process_Event methods.

- some of the methods in modules ipx.cpp and frame.cpp perform copying of data, where a constant _IPX_Packet_Data_Length is being the limit.
This constant was replaced at several places by constant _KST_Packet_Data_Length_Modified, which expresses the actual need of copying and there is no unneccessary data movement performed (of which most are unused characters anyway).
That fulfills the requirement where data transfer is done as per rule "1 character from keyboard = 1 data frame + 1 acknowledgement frame = 2 IPX packets", however the network capacity is drastically reduced.
in case of (possible future) file transmission mode when the whole capacity of the data field of the frame could this custom modification lead to data loss.
If that's the case, the constant swap needs to be reverted.

- program might be enhanced with functionality such as:
	- file transfer
	- multiple chat sessions in parallel
	- use of SPX protocol

- and optimization ideas are:
	- array of prioritiy indicators
	- better logic for acknowledgement algorithm
	- better way of exception handling
	- switch to higher resolution text mode (e.g. 80x50) and functions to move windows around, resize them or so
	- make the windows implementation more effective.

- implementation notes:
	- the program works with characters what have assumed values within 1 byte
	- implicit default character encoding is DOS CP437, which defines extended ASCII character set that includes characters for table borders


Sources:

During the development of this program I made use of know how on usage of the IPX for DOS driver, which as available in Czech language at these links:
	- http://www.cs.vsb.cz/grygarek/LAN/ipx.html (update 2025: still available)
	- http://petrfaltus.ic.cz/ipxspx.htm (update 2025: no longer available)

The finite state machine automation approach I learned from book:
	- Jan Rychlik - Programovaci techniky, KOPP 1994, ISBN 80-85828-05-7

Priceless information one use of C/C++ to develop under DOS gathered from books:
	- Pavel Herout - Ucebnice jazyka C, KOPP 1993, ISBN 80-85828-02-2
	- Herout, Rudolf, Smrha - ABC programatora v jazyce C, KOPP 1991, ISBN 80-901051-2-2

Great study materials for languages C and C++ are available here:
	- http://www.fit.vutbr.cz/study/courses/CPP/public/ (update 2025: no longer available)


Credits:
- Jan Cepela and Petr Jordan, who introduced me to the dark side of programming in C for DOS/x86 platform
- Lucie Vaclavkova for unmeasured patience and support while developing this program,
- firma GiTy Net, s.r.o., Czech Republic, for providing support and stage opportunity in the area of computer networking and communications




