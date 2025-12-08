// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991 to support translated files.
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <fstream.hpp>

ifstream::ifstream() : ios(&buffer) { clear(ios::badbit); }

ifstream::ifstream(int file_descriptor, int io_mode)
    : ios(&buffer)
{
    fstream_common::attach(file_descriptor, io_mode);
}

ifstream::ifstream(const char *name, int io_mode, int protection)
    : ios(&buffer)
{
    fstream_common::open(name, io_mode, protection);
}

ifstream::ifstream(int file_descriptor, char *memory, int length, int io_mode)
    : ios(&buffer)
{
    fstream_common::setbuf(memory, length);
    fstream_common::attach(file_descriptor, io_mode);
}

ifstream::~ifstream()
{
}
