// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream.hpp>

const int widest_real = 50;

ostream &ostream::operator<<(double value)
{
    const char *end;
    int field_width;
    int number_width;
    int negative;
    char buffer[widest_real+1];
    int exp;
 
    if (!opfx())
        return *this;

// If ios::fixed is set, convert using fcvt() otherwise use ecvt(), then
// check if the representation can be fixed up as fixed
    if (flags() & ios::fixed) {
fixed:
        number_width = fixed_point(value, buffer, negative);
    } else {
        number_width = sci_notation(value, buffer, exp, negative);
        if (!(flags() & ios::scientific) && exp > -4 && exp <= precision())
            goto fixed;
    }

    field_width = number_width;

    if (negative || (flags() & ios::showpos))
        field_width++;

    if (adjust(ios::right, field_width) == EOF)
        return *this;

    if (negative)
        if (rdbuf()->sputc('-') == EOF) {
            eof_fail();
            return *this;
        }
    else if (flags() & ios::showpos)
        if (rdbuf()->sputc('+') == EOF) {
            eof_fail();
            return *this;
        }

    if (adjust(ios::internal, field_width) == EOF)
        return *this;

    if ( rdbuf()->sputn(buffer, number_width) == EOF ) {
        eof_fail();
        return *this;
    }

    adjust(ios::left, field_width);

    osfx();
    return *this;
}

// Fixed point conversion using fcvt()
int ostream::fixed_point(double value, char *buf, int &negative)
{
    int point;
    char *s = buf;
    const char *string = fcvt(value, precision(), &point, &negative);
    if ( string == 0 || *string == '\0' ) {
        *s++ = '0';
        if (precision() && (flags() & ios::showpoint)) {
            *s++ = '.';
            for (int i = precision(); i--;)
                *s++ = '0';
        }
        *s = '\0';
        return s-buf;
    }

    int count;
    if ( point > 0 ) {
        count = point;
        while ( count-- > 0 && *string )
            *s++ = *string++;
    } else
        *s++ = '0';
// Put a leading zero before the decimal point

    char *decimal_point = s;
    *s++ = '.';
    if (point >= 0) {
        while (*string)
            *s++ = *string++;
    } else {
        count = -point;
        int zeroes = count > precision()? precision(): count;
        while (count--)
            *s++ = '0';
        if (zeroes < precision())
            while (*string)
                *s++ = *string++;
    }
    *s-- = '\0';
            
// If there is no fractional part and ios::showpoint is not set,
// do not show the decimal point or trailing zeroes.
    if (!(flags() & ios::showpoint)) {
        while ( s > decimal_point && *s == '0' )
            *s-- = '\0';
        if ( s == decimal_point )
            *s-- = '\0';
    }

    return s-buf+1;
}

int ostream::sci_notation(double value, char *buf, int &exp, int &negative)
{
// There should be a decimal point and at least one digit,
// unless the user has set precision to 0. Trailing zeroes,
// other than a zero immediately to the right of the decimal
// point, are truncated unless ios::showpoint is set.
    char *s = buf;
    const char *string =  ecvt(value, precision()+1, &exp, &negative);
    int point = exp;
    if ( string == 0 || *string == '\0' ) {
        *s++ = '0';
        if (precision() && (flags() & ios::showpoint)) {
            *s++ = '.';
            for (int i = precision(); i--;)
                *s++ = '0';
        }
        strcpy(s,(flags() & ios::uppercase)? "E+00": "e+00");
        return s-buf+4;
    }

    *s++ = *string++;

    char *decpoint = s;
    if (precision())
        *s++ = '.';
    if (*s)
        while (*string)
            *s++ = *string++;
    else
        if (precision() && (flags() & ios::showpoint))
            *s++ = '0';
    if (!(flags() & ios::showpoint)) {
        s--;
        while (s >= decpoint && (*s == '0' || *s == '.'))
            *s-- = '\0';
        s++;
    }


// Introduce the exponent. Always print a sign for the exponent.
    *s++ = (flags() & ios::uppercase)? 'E': 'e';

    if (value != 0.0)
        point--;

    *s++ = point < 0? (point = -point, '-'): '+';

    if (point >= 100) {
        *s++ = '0' + (point / 100);
        point %= 100;
    }
    *s++ = '0' + (point / 10);
    point %= 10;
    *s++ = '0' + point;

    *s = '\0';

    return s-buf;
}
