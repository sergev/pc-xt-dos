// STATICS.CPP
//
// Written by Steve Teale, April 1991
// Copyright Zortech 1991.
//
// This translation unit contains the code to initialize the
// standard stream classes, and to set up appropriate ties
// and defaults.
//
// In the case of compilers generating native code there is no
// need for complicated provisions to ensure that static objects
// with constructors get initialized before they are used.
//
// Library objects are initialized before user objects, The
// stream classes are currently the only such objects in the
// libraries.

#include <fstream.hpp>

filebuf __fd0(0, ios::in
#ifndef __UNIX__
                | ios::translated);
#endif
filebuf __fd1(1, ios::out
#ifndef __UNIX__
                | ios::translated);
#endif
filebuf __fd2(2,0,0, ios::out
#ifndef __UNIX__
                | ios::translated);
#endif
#ifdef __UNIX__
filebuf __fd3(2, ios::out);
#endif

istream_withassign cin(&__fd0);
ostream_withassign cout(&__fd1);
ostream_withassign cerr(&__fd2);
#ifdef __UNIX__
ostream_withassign clog(&__fd3);
#endif

IosTies __iosties;  // set up ties etc
