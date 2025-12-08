// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.
#include <disp.h>

#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <iostream.hpp>

// Insert a null-terminated character string.
ostream &ostream::operator<<(const char *string)
{
    if ( opfx() ) {
        int count = strlen(string);
        if (adjust(ios::right, count) == EOF)
            goto quit;
        if ( rdbuf()->sputn(string, count) != count) {
            eof_fail();
            goto quit;
        }
        adjust(ios::left|ios::internal, count);
    }
quit:
    osfx();

    return *this;
}

// Insert a single character.
ostream &ostream::operator<<(char c)
{
    if ( opfx() ) {
        if (adjust(ios::right, 1) == EOF)
            goto quit;
        if ( rdbuf()->sputc(c) == EOF ) {
            eof_fail();
            goto quit;
        }
        adjust(ios::left|ios::internal, 1);
    }
quit:
    osfx();

    return *this;
}

// Insert a void *
ostream &ostream::operator<<(void *vp)
{
    if (opfx()) {
        int i;
        char *p, buf1[10], buf2[20];
#ifdef DOS386
        itoa((unsigned) vp,buf1,16);
        i = 8 - strlen(buf1);
        p = buf2;
        *p++ = ':';
        for (; i--;)
            *p++ = '0';
        strcpy(p,buf1);
#else
#if M_I86SM || M_I86MM
        itoa((unsigned) vp,buf1,16);
        i = 4 - strlen(buf1);
        p = buf2;
        *p++ = ':';
        for (; i--;)
            *p++ = '0';
        strcpy(p,buf1);
#else
        itoa(FP_SEG(vp),buf2,16);
        p = buf2+strlen(buf2);
        *p++ = ':';
        itoa(FP_OFF(vp),p,16);
#endif
#endif
        int count = strlen(buf2);
        if (adjust(ios::right, count) == EOF)
            goto quit;
        if (rdbuf()->sputn(buf2, count) != count) {
            eof_fail();
            goto quit;
        }
        adjust(ios::left|ios::internal, count);
    }
quit:
    osfx();
    return *this;
}

// Insert a streambuf
ostream &ostream::operator<<(streambuf *s)
{
    int c, rv;

    while ((c = s->sbumpc()) != EOF && (rv = rdbuf()->sputc(c)) != EOF ) ;
    if (rv == EOF)
        eof_fail();
    return *this;
}

// Flush output
ostream &ostream::flush()
{
    if (rdbuf())
        rdbuf()->sync();
    return *this;
}

/*
 * Manipulators
 */

ostream &ends(ostream &stream)
{
    stream.put('\0');
    return stream;
}

ostream &flush(ostream &stream) // DJB
{
    stream.flush();
    return stream;
}

ostream &endl(ostream &stream)
{
    stream.put('\n');
    stream.flush();
    return stream;
}

ostream &stickywidth(ostream &stream)
{
    stream.setf(ios::stickywidth);
    return stream;
}

ostream &spacing(ostream &stream)
{
    stream.setf(ios::spacing);
    return stream;
}

ostream &nospacing(ostream &stream)
{
    stream.unsetf(ios::spacing);
    return stream;
}

ostream &fixed(ostream &stream)
{
    stream.setf(ios::fixed, ios::floatfield);
    return stream;
}

ostream &scientific(ostream &stream)
{
    stream.setf(ios::scientific, ios::floatfield);
    return stream;
}

ostream &showpoint(ostream &stream)
{
    stream.setf(ios::showpoint);
    return stream;
}

ostream &floating(ostream &stream)
{
    stream.unsetf(ios::floatfield | ios::showpoint);
    return stream;
}

ostream &leftjust(ostream &stream)
{
    stream.setf(ios::left,ios::adjustfield);
    return stream;
}

ostream &rightjust(ostream &stream)
{
    stream.setf(ios::right,ios::adjustfield);
    return stream;
}

ostream &internal(ostream &stream)
{
    stream.setf(ios::internal,ios::adjustfield);
    return stream;
}

ostream &showbase(ostream &stream)
{
    stream.setf(ios::showbase,0);
    return stream;
}
