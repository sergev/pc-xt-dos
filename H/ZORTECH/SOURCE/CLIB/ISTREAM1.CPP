// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

// Construct an istream associated with buffer.
istream::istream(streambuf * buffer) : ios(buffer), read_count(0) {}
istream::istream() : ios(0), read_count(0) {}

istream::~istream()
{
// There is a default constructor which sets a zero buffer
// pointer so must check for this.
    if (rdbuf())
        rdbuf()->sync();
}
