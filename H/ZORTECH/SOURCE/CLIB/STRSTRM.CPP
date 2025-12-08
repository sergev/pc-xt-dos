// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <string.h>
#include <strstrea.hpp>

istrstream::istrstream(char *memory, int length)
    : ios(&buffer), buffer(memory, length, 0) {}

istrstream::~istrstream() {}

ostrstream::ostrstream() : ios(&buffer) {}

ostrstream::ostrstream(char *memory, int length, int mode)
    : ios(&buffer), buffer(memory, length,
        (mode & (ios::app|ios::ate)) ?
            memory + strlen(memory) : memory) {}

ostrstream::~ostrstream() {}

char *ostrstream::str()
{
    clear(ios::eofbit|rdstate());
    return buffer.str();
}

void ostrstream::unfreeze()
{
    buffer.freeze(0);
    clear();
}

strstream::strstream()
: ios(&buffer) {}

strstream::strstream(char *memory, int length, int mode)
    :   ios(&buffer),
        buffer(memory, length >= 0? length: strlen(memory),
                (mode & (ios::app|ios::ate)) ?
                    memory + strlen(memory) : memory) {}

strstream::~strstream() {}

char *strstream::str()
{
    clear(ios::eofbit|rdstate());
    return buffer.str();
}

void strstream::unfreeze()
{
    buffer.freeze(0);
    clear();
}
