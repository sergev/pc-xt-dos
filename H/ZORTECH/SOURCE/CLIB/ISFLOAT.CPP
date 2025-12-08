// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#define _MT	1		// make sure we are thread-aware

#include <stdlib.h>
#include <iostream.hpp>

istream &istream::operator >> (float &value)
{
    double result;
    *this >> result;
    value = result;
    return *this;
}

class pattern {
private:
    char        buf[128];
    char *      s;
    istream *   stream;
public:
        pattern(istream * i) : stream(i), s(buf) {}
    int check();
    int accept(char c);
    int accept(char low, char high);
    char *  string();
};

int pattern::check()
{
    if ( s >= &buf[sizeof(buf)-1] ) {
        stream->clear(ios::badbit|ios::failbit|stream->rdstate());
// If the buffer overflows we are in serious deep ...

        return 0;
    }
    else if ( !stream->good() )
        return 0;
    else
        return 1;
}

int pattern::accept(char expected)
{
    if (check()) {
        char c  = stream->rdbuf()->sgetc();

        if ( c == EOF ) {
            stream->clear(ios::eofbit|stream->rdstate());
            return 0;
        }

        if (c == expected) {
            *s++ = c;
            stream->rdbuf()->stossc();
            return 1;
        }
    }
    return 0;
}

int pattern::accept(char low, char high)
{
    if (check()) {
        char c  = stream->rdbuf()->sgetc();

        if ( c == EOF ) {
            stream->clear(ios::eofbit|stream->rdstate());
            return 0;
        }

        if ( c >= low && c <= high ) {
            *s++ = c;
            stream->rdbuf()->stossc();
            return 1;
        }
    }
    return 0;
}

char *pattern::string()
{
    *s = '\0';
    return buf;
}

istream &istream::operator>>(double &value)
{
    pattern p(this);
    int got_a_digit = 0, got_a_sign = 0, got_point = 0,
        got_exp = 0, got_exp_digit = 0;

    if ( ipfx(0) ) {
        if ((got_a_sign = p.accept('+')) == 0)
            got_a_sign = p.accept('-');
        while (p.accept('0', '9'))
            got_a_digit = 1;
        if ((got_point = p.accept('.')) != 0) {
            while (p.accept('0', '9'))
                got_a_digit = 1;
        }
        if (got_a_digit) {
            if ( p.accept('e') || p.accept('E') ) {
                got_exp = 1;
                if ( !p.accept('+') )
                    p.accept('-');
                while ( p.accept('0', '9') )
                    got_exp_digit = 1;
                if (!got_exp_digit) {
                    clear(ios::badbit|ios::failbit|rdstate());
                    value = 0.0;
                    return *this;
                }
            }
        } else {
            if (got_a_sign || got_point)
                clear(ios::badbit|ios::failbit|rdstate());
            else 
                clear(ios::failbit|rdstate());
// did not lose characters
            value = 0.0;
            return *this;
        }
        char *dummy;
        value = strtod(p.string(), &dummy);
        if (errno == ERANGE) {
            errno = 0;
            clear(ios::badbit|ios::failbit|rdstate());
            value = 0.0;  // probably should be a NAN
            return *this;
        }
    }
    else
        value = 0.0;
    return *this;
}
