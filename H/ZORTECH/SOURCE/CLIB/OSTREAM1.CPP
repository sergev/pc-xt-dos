// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.
#include <disp.h>

#include <string.h>
#include <iostream.hpp>

ostream::ostream(streambuf * buffer) : ios(buffer) {}
ostream::ostream() : ios(0) {}

ostream::~ostream()
{
// There is a default constructor which sets a zero
// buffer pointer, must check for this.
    if (rdbuf())
        rdbuf()->sync();
}
