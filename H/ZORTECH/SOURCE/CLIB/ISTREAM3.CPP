// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

// Extract up to "length" characters, and store them in "data". Stop extraction
// after the delimiter character, extracting the delimiter.
istream &istream::getline(char *data, int length, char delimiter)
{
    read_count = 0;
    int c;

    if ( ipfx(1) ) {
        for (--length; length--;) {
            c = rdbuf()->sgetc();
            if ( c == EOF ) {
                clear(eofbit|badbit|rdstate());
                break;
            }
            *data++ = c;
            rdbuf()->stossc();
            read_count++;
            if (c == delimiter)
                break;
        }
    }
    *data = '\0';
    if (!read_count && c == EOF)
        clear(failbit|rdstate());
    return *this;
}

// Same as above, but for unsigned character data.
istream &istream::getline(unsigned char *data, int length, char delimiter)
{
    return getline((char *) data, length, delimiter);
}

// Throw away up to "length" characters. Stop after a character is equal
// to "delimiter".
istream &istream::ignore(int length, int delimiter)
{
    if ( ipfx(1) ) {
        int c;

        while ( length--
        && (c = rdbuf()->sgetc()) != EOF
        && c != delimiter ) ;

        if ( c == EOF )
            clear(eofbit|rdstate());
    }
    return *this;
}
