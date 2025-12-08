// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.
#include <disp.h>

#include <string.h>
#include <iostream.hpp>

// Insert a single character.
ostream &ostream::put(char c)
{
    if ( opfx() ) {
        if ( rdbuf()->sputc(c & 0xff) == EOF )
            eof_fail();
    }
    return *this;
}

// Perform an absolute seek.
ostream &ostream::seekp(streampos position)
{
    if ( rdbuf()->seekpos(position, ios::out) == EOF )
        eof_fail();
    return *this;
}

// Perform a relative seek.
ostream &ostream::seekp(streamoff position, seek_dir direction)
{
    if ( rdbuf()->seekoff(position, direction, ios::out) == EOF )
        eof_fail();
    return *this;
}

// Return the current file position.
streampos ostream::tellp()
{
    streampos result = rdbuf()->seekoff(0, cur, ios::out);

    if ( result == EOF )
        eof_fail();
    return result;
}

// Write a string of n characters without formatting.
ostream &ostream::write(const void *data, size_t size)
{
    if ( (write_count = rdbuf()->sputn((const char *) data, size))
                                != size )
        eof_fail();
    return *this;
}
