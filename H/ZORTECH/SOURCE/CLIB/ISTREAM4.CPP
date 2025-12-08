// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

// Extract into a character array until a white-space character is seen.
// If width() is non-zero, it is taken as
// the size of the array, and no more than width-1 characters will be
// extracted. A terminating null is always stored, even if nothing else
// is done because of the error state.
istream &istream::operator>>(char *data)
{
    int got_a_character = 0, length = width();
    length = length? length-1: INT_MAX-1;
    if (!(flags() & ios::stickywidth))
        width(0);   // reset width if not sticky

    if ( ipfx(0) ) {
        int c = 0;

        while (length--
                && (c = rdbuf()->sgetc()) != EOF
                && !isspace(c)) {
            *data++ = c;
            rdbuf()->stossc();
            got_a_character = 1;
        }
        if ( c == EOF )
            clear(eofbit|badbit|rdstate());
    }

// Set fail if zero characters extracted
    if (!got_a_character)
        clear(failbit|rdstate());

    *data = '\0';

    return *this;
}

// Extract a single character.
istream &istream::operator>>(char &c)
{
    int t;

    if (ipfx(0)) {
        if ((t = rdbuf()->sbumpc()) == EOF)
            clear(eofbit|failbit|badbit);
        else 
            c = t;
    }
    return *this;
}

// Extract characters and stuff them into the argument streambuf.
// Stop on EOF, or if the put to the streambuf fails.
istream &istream::operator>>(streambuf *destination)
{
    int got_a_character = 0;

    if ( ipfx(1) ) {
        int c;

        while ( (c = rdbuf()->sgetc()) != EOF ) {

// Stop if the receiving streambuf is full
            if ( destination->sputc(c) == EOF )
                return *this;

            rdbuf()->stossc();
        }
        clear(eofbit|rdstate());
    }
    return *this;
}
