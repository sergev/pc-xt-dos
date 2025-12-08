// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified by Steve Teale April 1991
// Copyright 1990-1991 Zortech Inc. All Rights Reserved.

#include <string.h>
#include <iostream.hpp>

const char default_padding_character = ' ';
const long ios::stickywidth = 0x8000;
const long ios::spacing = 0x10000;

ios::ios(streambuf * buffer)
    :   buf(buffer),
        format_state(ios::defaults),
        error_state(0),
        tied_ostream(0),
        padding_character(default_padding_character),
        decimal_precision(_ios_default_decimal_precision),
        field_width(0)
{
    memset(extended_format_words, 0, sizeof(extended_format_words));
}

ios::~ios()
{
// Do nothing virtual destructor
}

int ios::width(int new_value)
{
    int old_value = field_width;
    field_width = new_value < 0? 0: new_value;
    unsetf(ios::stickywidth);
    return field_width;
};


// Allocate an unused bit in the format state word.
// Returns a word with one bit set.
int ios::bitalloc()
{
    static int longbit = (ios::spacing << 1);
    int old_value = longbit;

    longbit <<= 1;
    return old_value;
// returns 0 when all bits have been allocated
}

// Allocate the next available index into the format words
// array. Returns EOF if no more available.
int ios::xalloc()
{
    static int index = 0;
    if (index >= _ios_n_extended_format_words)
        return EOF;
    int old = index;
    ++index;
    return old;
}

ios &dec(ios &stream)
{
    stream.setf(ios::dec, ios::basefield);
    return stream;
}

ios &hex(ios &stream)
{
    stream.setf(ios::hex, ios::basefield);
    return stream;
}

ios &oct(ios &stream)
{
    stream.setf(ios::oct, ios::basefield);
    return stream;
}

ios &defaults(ios &stream)
{
    stream.flags(ios::defaults);
    return stream;
}

IosTies::IosTies()
{
    cin.tie(&cout);
    cerr.tie(&cout);
}
