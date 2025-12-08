// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991 to support translated files.
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <fstream.hpp>

fstream_common::fstream_common() : ios(&buffer) {}

void fstream_common::attach(int file_descriptor, int io_mode)
{
    filebuf *fb = buffer.attach(file_descriptor, io_mode);
    if (!fb)
        clear(ios::failbit | ios::badbit | rdstate());
    else
        clear();
}

void fstream_common::close()
{
    filebuf *fb = buffer.close();
    if (!fb)
        clear(ios::failbit | ios::badbit | rdstate());
}

void fstream_common::open(const char *name, int io_mode,
            int protection)
{
    filebuf *fb = buffer.open(name, io_mode, protection);
    if (!fb)
        clear(ios::failbit | ios::badbit | rdstate());
    else
        clear();
}
