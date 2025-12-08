// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

// Peek at the next character in the input stream without extracting it.
// Saves one from having to use putback().
int istream::peek()
{
    if ( ipfx(1) ) {
        int result = rdbuf()->sgetc();

        if (result == EOF)
            clear(eofbit|badbit|rdstate());

        return result;
    }
    else
        return EOF;
}

// Push the last character read back onto the input stream.
istream &istream::putback(char c)
{
    if (good()) {
        if ( rdbuf()->sputbackc(c) == EOF )
            clear(failbit|rdstate());
    }
    return *this;
}

// Read "length" characters into "data". If less than "length" can be
// read, get what I can, set failbit, and the length actually read
// is available from gcount().
istream &istream::read(void *data, int length)
{
    if (ipfx(1)) {
        read_count = rdbuf()->sgetn(data, length);
        if ( read_count < length )
            clear(failbit|rdstate());
    }
    else
        read_count = 0;

    return *this;
}

// Absolute-seek the input character stream to "position".
istream &istream::seekg(streampos position)
{
    int rv = rdbuf()->seekoff(streamoff(position), ios::beg, ios::in);
    if (rv == EOF)
        clear(eofbit|failbit|rdstate());
    return *this;
}

// Relative-seek the input character stream to "position".
istream &istream::seekg(streamoff offset, relative_to rt)
{
    int rv = rdbuf()->seekoff(offset, rt, ios::in);
    if (rv == EOF)
        clear(eofbit|failbit|rdstate());
    return *this;
}

// Establish consistency between the internal data structure and the
// external character source. This usually means that
// characters that were buffered for input are thrown away, and
// the file pointer is decremented so that it appears that the
// buffered characters were never read.
int istream::sync()
{
    if ( rdbuf()->sync() == EOF ) {
        clear(failbit|rdstate());
        return EOF;
    }
    return 0;
}

// Return the current file position
streampos istream::tellg()
{
    return rdbuf()->seekoff(0, ios::cur, ios::in);
}
