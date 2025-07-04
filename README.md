# kst

This is a demo of using C++ to implement a chat over IPX on DOS platform on x86 architecture.

It's an old program but it demonstrates my favourite part of programming - network communication in C.

Even though there is no use for it, I'd still like sharing it, just for the fun of it.

It was a requirement to deliver this project in order to pass study course in Network Communication at my university study and I enjoyed working on this project very much.

My colleagues chose Pascal as implementation tool, but I took a chance and picked C, actually C++, because not only it's my favorite programming language, but especially because it's fun.
It is also extremely flexible, simple, but elegant, fast and powerful.
Easily integrates with assembler and allows to implement pretty much anything you can think of.

Overall I had great time working on this project.

You can still get all the requirements to build and run this program.

See here for instructions on getting and setting up Borland C++ in DosBox:

https://github.com/johangardhage/dos-bcdemos

Once you have that set up, you make sure that the DosBox console has ipx on.
Then in one of the terminals enable ipx:

ipxnet startserver

And in the other terminal (running on the same host) connect to the previosu terminal:

ipxnet connect 127.0.0.1

And then you have two terminals where you can experiment with KST.

In order to build the project using Borland C++ 3.1, use the attached PRJ files.
They carry the information on how to build the project and which files to use.
It also contains directory paths, so check that first if build fails, especially the path to the Borland C++ and to your own project on the DosBox virtual disk C:

If you can't or don't want to get the build environment up and running, you can still use the attached .EXE.
They would run in DosBox with IPX set up as per the above initialization commands.
The .EXE files are built with full logging capability, and they will produce plenty of logging in the current folder.

If you use the same DosBox virtual drive in each of the DOsBox terminals (as per above), make sure you create two folders for each instance, otherwise they gonna overwrite each other's log.
