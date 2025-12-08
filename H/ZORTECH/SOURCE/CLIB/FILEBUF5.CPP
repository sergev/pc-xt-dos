// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991 to allow translated (DOS style) IO
// Copyright Zortech 1990-1991. All Rights Reserved.
#include <disp.h>

#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <fstream.hpp>

// This is called by sputbackc() when the get pointer is equal to
// the base of the get area, and there is thus no space to put back
// characters. It may try to re-arrange the buffer so that it is
// possible to put back characters, and then put back c. If it
// succeeds it should return c, otherwise it should return EOF.

int filebuf::pbackfail(int c)
{
    if (c == EOF)           // can't put back EOF
        return EOF;
    if (gptr() <= eback()) {  // genuine fail
        if (!gptr()) {          // no get area set up
            if (!base()) {
                if ((allocate()) == EOF) return EOF;
                setp(0,0);  // One thing at a time
            } else {
                if (out_waiting()) {
                    if (syncout() == EOF)
                        return EOF;
                }
            }
// set up minimal get area
            setg(base(),base(),base()+1);
            *gptr() = c;
            return 0;
        }
        if (gptr_)          // already using the reserve
            return EOF;
// switch to putback reserve buffer
        char *end = pushback_buf+sizeof(pushback_buf);
        gptr_ = gptr();
        egptr_ = egptr();
        setg(pushback_buf, end, end);
    }
    gbump(-1);
    *gptr() = c;
    return 0;
}
