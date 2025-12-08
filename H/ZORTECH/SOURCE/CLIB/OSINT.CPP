// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <iostream.hpp>

const int widest_integer = 50;

// Insert a signed long integer.
ostream &ostream::operator<<(long value)
{

    if ( value >= 0 )
        return convert_unsigned(value, 0);
    else
        return convert_unsigned(-value, 1);
}

// Insert a unsigned long integer.
ostream &ostream::operator<<(unsigned long value)
{
    return convert_unsigned(value, 0);
}

// Convert the absolute value of the number into a string.
// If the number base is greater than 10, interpret the ios::uppercase
// bit of flags() to determine if a..z or A..Z should be used
// to represent numerals.
// This really only needs to work for base 8, 10, and 16, given the
// current definition of iostreams.
const char *ostream::unsigned_to_string(unsigned long value,
                    int radix, int &num_width)
{
    static char buf[widest_integer+1];
    register char *string = &buf[widest_integer];

    *string = '\0';

    if ( radix <= 10 ) {
        do {
            *(--string) = '0' + (char)(value % radix);
        } while ( value /= radix );
    }
    else { /* radix > 10 */
        do {
            register int    d = (int)(value % radix);

            if ( d < 10 )
                *(--string) = '0' + d;
            else if ( (flags() & ios::uppercase) == 0 )
                *(--string) = 'a' - 10 + d;
            else /* uppercase */
                *(--string) = 'A' - 10 + d;
        } while ( value /= radix );
    }

// How wide is the numeric string?
    num_width = &buf[widest_integer] - string;

    return string;
}

// Convert an unsigned long to a string, interpreting the radix indicators
// and uppercase. This takes the value and the sign in separate arguments
// so that I can have one routine that will print the entire range of signed
// and unsigned integers.
ostream &ostream::convert_unsigned(unsigned long value, int negative)
{
    register int radix;
    int rep_width, number_width;
    const char *string;
 
    if ( !opfx() )
        return *this;

    /*
     * Figure out the radix.
     */
    switch ( flags() & ios::basefield ) {
    case ios::oct:
        radix = 8;
        break;
    case ios::hex:
        radix = 16;
        break;
    default:
    case ios::dec:
        radix = 10;
        break;
    };

    string = unsigned_to_string(value, radix, number_width);
    rep_width = number_width;

    if ( width() ) {

// I get here if the user has set width(), and I have
// to calculate the amount of padding, etc.
//      * If I have to show the sign, the numeric string is one
// character longer. Note that ios::showpos applies only
// to _decimal_ output.
        if ( negative || ((flags() & ios::showpos) && radix == 10) )
            rep_width++;

// If I have to show the base, the numeric string gets longer,
// too.
        if ( flags() & ios::showbase ) {
            switch ( radix ) {
            case 8:
                rep_width++;
                break;
            case 16:
                rep_width += 2;
                break;
            default:
                break;
            }
        }
        adjust(ios::right, rep_width);
    }


// Show the sign.
// Note that ios::showpos only applies to decimal output.
    if ( negative )
        put('-');
    else if ( flags() & ios::showpos && radix == 10 )
        put('+');

    /*
     * Show the base if required.
     */
    if ( flags() & ios::showbase ) {
        switch ( radix ) {
        case 8:
            put('0');
            break;
        case 16:
            if ( rdbuf()->sputn(
            (flags() & ios::uppercase) ? "0X" : "0x", 2) == EOF ) {
                eof_fail();
                return *this;
            }
            break;
        default:
            break;
        }
    }

// Internal adjustment goes AFTER the sign and radix
// indicator, so that if the padding character is "0" (fill
// with leading zeroes) the number will come out right.
//      * If you want numbers to have leading zeroes, use fill('0'),
// setf(ios::internal), and set width().
    adjust(ios::internal, rep_width);

// Show the absolute value.
    if ( rdbuf()->sputn(string, number_width) == EOF ) {
        eof_fail();
        return *this;
    }

    adjust(ios::left, rep_width);

    osfx();
    return *this;
}
