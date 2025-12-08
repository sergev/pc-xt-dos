// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#define _MT	1		// make sure we are thread-aware

#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream.hpp>

const int octhex = ios::oct | ios::hex;

unsigned long istream::convert_integer(int &negative, int &uns)
{
    int     radix = 10;
    unsigned long   value = 0;
    int     got_a_numeral = 0, got_a_sign = 0, got_a_prefix = 0;
    char buf[20], *p = buf;
    int     c;

    if ( ipfx(0) == 0 )
        return 0;

    uns = negative = 0;
    switch ( c = rdbuf()->sgetc() ) {
    case '-':
        negative = 1;
    case '+':
        rdbuf()->stossc();
        got_a_sign = 1;
        break;
    case EOF:
        clear(ios::badbit|ios::failbit|ios::eofbit);
        return 0;
    default:
        break;
    }

    switch ( flags() & ios::basefield ) {
    case ios::hex:
        radix = 16;
        uns = 1;
        break;
    case ios::dec:
        radix = 10;
        break;
    case ios::oct:
        radix = 8;
        uns = 1;
        break;
    default:
        radix = 10;
        c = rdbuf()->sgetc();

// Check for the leading zero that would start a radix
// specifier.
        if ( c == '0' ) {
            *p++ = c;
            got_a_numeral = 1;
            rdbuf()->stossc();
            if ( (c = rdbuf()->sgetc()) == 'x'
                        || c == 'X' ) {
                *p++ = 'x';
                rdbuf()->stossc();
                radix = 16;
                uns = 1;
                got_a_prefix = 1;
                got_a_numeral = 0;  // just a prefix
                break;
            } else {
                radix = 8;
                uns = 1;
                break;
            }
        }

// I get here if the last character I read did not fit a radix
// specifier.
        if ( c == EOF ) {
            clear(ios::badbit|ios::failbit|ios::eofbit);
        }
        break;
    }

    for (;;) {
        c = rdbuf()->sgetc();
        if (radix == 16 && isxdigit(c))
            *p++ = c;
        else if (isdigit(c)) {
            if (radix == 8 && c > '7')
                break;
// What is correct and what is sensible?
//                clear(ios::failbit|ios::badbit|rdstate());
            *p++ = c;
        } else break;
        got_a_numeral++;
        rdbuf()->stossc();
    }
    if (good())
        *p = '\0';
    else
        *buf = '\0';
    value = strtoul(buf,&p,radix);

    if ( !got_a_numeral ) {
        if (got_a_sign || got_a_prefix)     // lost a character
            clear(ios::badbit|ios::failbit|rdstate());
        else 
            clear(ios::failbit|rdstate());
    }

    if ( c == EOF )
        clear(ios::eofbit|rdstate());

    return value;
}

istream &istream::operator>>(signed char &value)
{
    int negative, uns;
    unsigned long ur = convert_integer(negative, uns);
    if (errno == ERANGE) {
        errno = 0;
        goto ovf;
    }
    if (uns) { // representation was unsigned
        if (ur > (unsigned long) UCHAR_MAX)
            goto ovf;
    } else if (ur == ((unsigned long) SCHAR_MAX)+1 && negative) {
        value = -SCHAR_MAX; value--;
        return *this;
    } else if (ur > (unsigned long) (SCHAR_MAX)) {
ovf:
        value = SCHAR_MAX;
        if (negative) value = -value-1;
        clear(ios::failbit|rdstate());
        return *this;
    }

    value = ur;
    value = negative? -value: value;

    return *this;
}

istream &istream::operator>>(short &value)
{
    int negative, uns;
    unsigned long ur = convert_integer(negative, uns);
    if (errno == ERANGE) {
        errno = 0;
        goto ovf;
    }
    if (uns) { // representation was unsigned
        if (ur > (unsigned long) USHRT_MAX)
            goto ovf;
    } else if (ur == ((unsigned long) SHRT_MAX)+1 && negative) {
        value = -SHRT_MAX-1;
        return *this;
    } else if (ur > (unsigned long) (SHRT_MAX)) {
ovf:
        value = SHRT_MAX;
        if (negative) value = -value-1;
        clear(ios::failbit|rdstate());
        return *this;
    }

    value = ur;
    value = negative? -value: value;

    return *this;
}

istream &istream::operator>>(int &value)
{
    int negative, uns;
    unsigned long ur = convert_integer(negative, uns);
    if (errno == ERANGE) {
        errno = 0;
        goto ovf;
    }
    if (uns) { // representation was unsigned
        if (ur > (unsigned long) UINT_MAX)
            goto ovf;
    } else if (ur == ((unsigned long) INT_MAX)+1 && negative) {
        value = -INT_MAX-1;
        return *this;
    } else if (ur > (unsigned long) (INT_MAX)) {
ovf:
        value = INT_MAX;
        if (negative) value = -value - 1;
        clear(ios::failbit|rdstate());
        return *this;
    }

    value = ur;
    value = negative? -value: value;

    return *this;
}

istream &istream::operator>>(long &value)
{
    int negative, uns;
    unsigned long ur = convert_integer(negative, uns);
    if (errno == ERANGE) {
        errno = 0;
        goto ovf;
    }
    if (!uns && (ur == ((unsigned long) LONG_MAX)+1 && negative)) {
        value = -LONG_MAX-1;
        return *this;
    } else if (!uns && ur > (unsigned long) (LONG_MAX)) {
ovf:
        value = LONG_MAX;
        if (negative) value = -value - 1;
        clear(ios::failbit|rdstate());
        return *this;
    }
    value = ur;
    value = negative? -value: value;
    return *this;
}

istream &istream::operator>>(unsigned char &value)
{
    unsigned long result;
    volatile unsigned char conversion;

    *this >> result;

    conversion = (unsigned char) result;

    if ( result != (unsigned long) conversion ) {
        value = UCHAR_MAX;
        clear(ios::failbit|rdstate());
    } else 
        value = conversion;

    return *this;
}

istream &istream::operator>>(unsigned short &value)
{
    unsigned long       result;
    volatile unsigned short conversion;

    *this >> result;

    conversion = (unsigned short)result;

    if ( result != (unsigned long)conversion ) {
        value = USHRT_MAX;
        clear(ios::failbit|rdstate());
    } else 
        value = conversion;

    return *this;
}

istream &istream::operator>>(unsigned int &value)
{
    unsigned long result;
    volatile unsigned int conversion;

    *this >> result;

    conversion = (unsigned int)result;

    if ( result != (unsigned long)conversion ) {
        value = UINT_MAX;
        clear(ios::failbit|rdstate());
    } else
        value = conversion;

    return *this;
}

istream &istream::operator>>(unsigned long &value)
{
    int negative, uns;
    value = convert_integer(negative, uns);
    if (errno == ERANGE) {
        errno = 0;
        value = ULONG_MAX;
        clear(ios::failbit|rdstate());
        return *this;
    }
    if (negative)
        clear(ios::failbit|rdstate());
    return *this;
}
