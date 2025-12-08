// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.
#include <disp.h>

#include <string.h>
#include <iostream.hpp>

void ostream::eof_fail()
{
    clear(ios::badbit | ios::failbit | ios::eofbit);
}

// Insert padding for the numeric and string inserters.
//
// Adjustment is done only if width() is non-zero; "kind" is a
// bit-mask indicating the kind of adjustment to do. It is one or
// more of ios::left, ios::right, and ios::internal.
//
// If none of the bits of ios::adjustfield are set, the default is
// right-adjustment. Return non-zero if there is an error.
int ostream::adjust(int kind, int rep_width)
{
    if (width()) {  // user has set width
        if ( flags() & kind
                || (kind & ios::right && flags() == 0) ) {
            int padding = width() - rep_width;

            while ( padding-- > 0) {
                if ( rdbuf()->sputc(fill()) == EOF ) {
                    eof_fail();
                    return EOF;
                }
            }
        }
    }
    return 0;
}

// Perform output-prefix operations.
//
// If the error state is non-zero, return zero immediately.
// If there is a tied ostream (see ios::tie()), flush it.
// Return non-zero. If you define your own inserter that directly
// manipulates the streambuf instead of calling other inserters,
// it should call this when it starts.
int ostream::opfx()
{
    if (good()) {
        if (tie()) 
            tie()->flush();
        return 1;
    }
    return 0;
}

// output-suffix operations.
//
// If ios::unitbuf is set in the format state, flush output.
// If ios::stdio is set, flush stdio and stderr. If you define
// your own inserter that directly manipulates the streambuf
// it should call this just before returning. All of the inserters
// in ostream call this. Binary put() and write() functions do
// not call this.
void ostream::osfx()
{
    if (flags() & ios::spacing) {
        if (rdbuf()->sputc(fill()) == EOF)
            eof_fail();
    }
    if (!(flags() & ios::stickywidth))
        width(0);
    if ( flags() & ios::unitbuf )
        flush();
    if (flags() & stdio)
        flush_stdio();
}
