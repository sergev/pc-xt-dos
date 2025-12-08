// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

// Extract up to "length" characters, and store them in "data". Stop extraction
// at the delimiter character without extracting the delimiter.
istream &istream::get(char *data, int length, char delimiter)
{
    read_count = 0;
    int c = 0;

    if ( ipfx(1) ) {
        for ( --length; length--
        && (c = rdbuf()->sgetc()) != delimiter;) {
            if ( c == EOF ) {
                clear(eofbit|badbit|rdstate());
                break;
            }
            *data++ = c;
            rdbuf()->stossc();
            read_count++;
        }
    }
    *data = '\0';
    if (!read_count  && c == EOF)
        clear(failbit|rdstate());
    return *this;
}

istream & istream::get(unsigned char *data, int length, char delimiter)
{
    return get((char *) data, length, delimiter);
}

istream &istream::get(char &c)
{
    int t = get();
    if (t == EOF)
        clear(eofbit|failbit|badbit);
    else
        c = t;
    return *this;
}

istream &istream::get(signed char &c)
{
    int t = get();
    if (t == EOF)
        clear(eofbit|failbit|badbit);
    else
        c = t;
    return *this;
}

istream &istream::get(unsigned char &c)
{
    int t = get();
    if (t == EOF)
        clear(eofbit|failbit|badbit);
    else
        c = t;
    return *this;
}

// Extract characters, and stuff them into the argument streambuf.
// Stop extraction at the delimiter character without extracting the delimiter.
// Otherwise, stop extraction if EOF is encountered or the put to the
// streambuf fails.
istream &istream::get(streambuf &destination, char delimiter)
{
    read_count = 0;

    if ( ipfx(1) ) {
        int c;

        while ( (c = rdbuf()->sgetc()) != delimiter ) {
            if ( c == EOF ) {
                clear(read_count? (eofbit|badbit|rdstate()):
                                        (eofbit|failbit|badbit));
                return *this;
            }

// Make sure that read count reflects what was actually transferred
            if ( destination.sputc(c) == EOF )
                break;

            rdbuf()->stossc();
            read_count++;
        }
    }
    return *this;
}

int istream::get()
{
    int t;

    if ( ipfx(1) ) {
        if ((t = rdbuf()->sbumpc()) != EOF ) {
            read_count = 1;
            return t;
        }
        clear(eofbit|failbit|badbit);
    }
    read_count = 0;
    return EOF;
}
