// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <disp.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <strstrea.hpp>

void strstreambuf::buffer_setup(char *memory, int length,
                                    char *start_of_put_area)
{
    char *end;

    if ( length > 0 )
        end = memory + length;
    if ( length < 0 )
        end = (char *) (UINT_MAX);
    else if ( length == 0 )
        end = memory + strlen(memory)+1;
// take account of terminating null byte

    setb(memory, end, 0);

    if (start_of_put_area) {
        if ( start_of_put_area < memory
        || start_of_put_area > end ) {
            errno = EINVAL;
            setg(0,0,0);
            setp(0,0);
            return;
        }

        if (sflags&statmem && start_of_put_area > memory)
// Assume there is something useful in the get area.
            setg(memory, memory, start_of_put_area);
        else
// there is nothing to get.
            setg(memory,memory,memory);
        setp(start_of_put_area, end);
    } else {
        sflags |= rdonly;
        setg(memory, memory, end);
        setp(0,0);
    }
}

strstreambuf::strstreambuf(char *memory, int length,
                                char *start_of_put_area)
    :   sflags(statmem)
{
    buffer_setup(memory, length, start_of_put_area);
}

strstreambuf::strstreambuf(unsigned char *memory, int length,
                            unsigned char *start_of_put_area)
    :   sflags(statmem)
{
    buffer_setup((char *) memory, length, (char *) start_of_put_area);
}

strstreambuf::strstreambuf(int chunksize)
    :   sflags(merged),
        chunk(chunksize),
        allocate_function(operator new),
        free_function(operator delete)
{}

strstreambuf::strstreambuf(void *(*allocate_f)(size_t),
                void (*free_f)(void *), int chunksize)
    :   sflags(merged),
        chunk(chunksize),
        allocate_function(allocate_f),
        free_function(free_f)
{}

strstreambuf::~strstreambuf()
{
    if (!(sflags & (statmem|frozen)) && base() )
        (*free_function)(base());
}

void strstreambuf::freeze(int on)
{
    if (on)
        sflags |= frozen;
    else
        sflags &= ~frozen;
}

char *strstreambuf::str()
{
    freeze();
// Add a safety net if there is room
    if (pptr() < ebuf())
        *pptr() = '\0';

    return base();
}

streambuf *strstreambuf::setbuf(char *, int length)
{
    chunk = length;
    return this;
}

int strstreambuf::doallocate()
{
    if (sflags & statmem)
        return EOF;

    int length = chunk;
    char * memory;

    if ( length <= 0 )
        length = default_allocation;
    memory = (char *) (*allocate_function)(length);
    if (!memory)
        return EOF;

    buffer_setup(memory, length, memory);
    return 0;
}

int strstreambuf::overflow(int c)
{
    if (sflags & (statmem|frozen|rdonly))
        return EOF;
    if (!base()) {
        if (allocate() == EOF)
            return EOF;
        if (c != EOF) {
            *pptr() = c;
            pbump(1);
        }
        return 0;
    } else {

        char *memory, *old = base();
        unsigned new_length = blen() * 2 + blen();

        memory = (char *)(*allocate_function)(new_length);
        if (!memory)
            return EOF;

        memmove(memory, base(), blen());

// Relax the get restriction while we are at it
        setg(memory, memory+(gptr()-base()),
                    memory+(pptr()-base()));
        int pbl = pptr()-base();
        setp(memory, memory+new_length);
// Position pptr relative to new buffer
        pbump(pbl);
        setb(memory, memory+new_length, 0);

        if ( c != EOF ) {
            *pptr() = c;
            pbump(1);
        }

        (*free_function)(old);

        return 0;
    }
}

int strstreambuf::underflow()
{
    if (sflags&merged) {
// Try to extend the get area to overlap characters which have been put.
        if (gptr() && egptr() < pptr()) {
            unsigned t = egptr()-eback();
            setg(base(),base()+t,pptr());
        } else
            return EOF;
        return (unsigned char) *gptr();
    }
// Otherwise there is no more
    return EOF;
}

streampos strstreambuf::seekoff(streamoff offset, relative_to from, int which)
{
    char * pointer;
    if (offset.stepsize() != 1)
        return EOF;
    long diff = offset.steps();
    if (diff > INT_MAX || diff < INT_MIN)
        return EOF;
    int d = int(diff);

    underflow();
// Relax any unneccessary gptr restriction
    if (which == ios::in) {
        switch (from) {
        case ios::beg:
            pointer = base() + d;
            break;
        case ios::cur:
            pointer = gptr() + d;
            break;
        case ios::end:
            pointer = egptr() + d;
            break;
        default:
            return EOF;
        }
        if ( pointer < base() || pointer > egptr() )
            return EOF;
        setg(base(), pointer, egptr());
        return pointer - base();
    }
    else if ( which & ios::out ) {
        if (!pptr())
            return EOF;
        switch (from) {
        case ios::beg:
            pointer = pbase() + d;
            break;
        case ios::cur:
            pointer = pptr() + d;
            break;
        case ios::end:
            pointer = epptr() + d;
            break;
        default:
            return EOF;
        }
        if (pointer < pbase() || pointer > epptr())
            return EOF;

        if (which & ios::in) {

// It isn't defined what to do if the user tries to seek both
// pointers of a strstreambuf. What happens here is that the get
// area is extended to cover any characters which have been put,
// so the buffer becomes more general purpose. Characters can then
// be got from anywhere in the buffer.  Puts are still restricted
// to the designated area. Only puts can extend the buffer however.
//
// The str() function will return a pointer to the whole buffer.
// Dynamically allocated buffers behave like this all the time.
            if ( pointer < base() || pointer > pptr() )
                return EOF;
            sflags |= merged;
            setg(base(), pointer, pptr());
        }

        pbump(pointer - pptr());
        return pptr() - pbase();
    }
    else
        return EOF;
}

int strstreambuf::sync()
{
    return 0;   // always in sync by definition
}
