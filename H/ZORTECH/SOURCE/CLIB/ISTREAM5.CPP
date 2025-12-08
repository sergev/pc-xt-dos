// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <ctype.h>
#include <limits.h>
#include <iostream.hpp>

int istream::ipfx(int need)
{
    if ( !good() )
        return 0;

// If there is a tied ostream, flush it.
// The draft ansi spec says if need <= characters than are
// currently available in the input stream, don't bother, but
// this doesn't make sense, since it also states that ipfx is
// called with argument 0 or 1. So this implementation flushes
// it regardless.
    if (tie())
        tie()->flush();

    if (!need && (flags() & skipws)) {
        int c;

// Ellis and Stroustrup 2.1 says that white-space is:
// Blanks, horizontal and vertical tabs, newlines, formfeeds,
// and comments. Comments don't apply here. They don't mention
// carriage-return, but they are not being character-set
// specific.
        while ((c = rdbuf()->sgetc()) != EOF) {
            if (isspace(c)) {
                rdbuf()->stossc();
                continue;
            }
            else {
                return 1;
            };
        }
// EOF, so stream no longer useable, but not a fail!
        clear(eofbit|badbit|rdstate());
        return 0;
    }
    return 1;
}

istream &ws(istream &stream)
{
    stream.ipfx(0);
    return stream;
}
