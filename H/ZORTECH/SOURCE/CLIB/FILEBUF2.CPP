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

// set up a lookup table from the C header file
const int filebuf::lseek_consts[3] = { SEEK_SET, SEEK_CUR, SEEK_END };

// Perform a relative seek of the get or put pointers, or both, to
// offset. "which" signifies what pointer is to be affected: ios::in
// for the get pointer, ios::out for the put pointer. In the case of
// a filebuf, the two are usually stuck together.
//
streampos filebuf::seekoff(streamoff offset, relative_to from, int)
{

// Seeking on device which does not support it is an error.
    if ( do_not_seek )
        return streampos(EOF);

    sync(); // eliminate the effects of buffering
    if (offset.stepsize() != 1) return streampos(EOF);
                    // Implementation restriction
    long t = offset.steps();

    if ((t = lseek(file, t, lseek_consts[from])) != -1L)
        return streampos(t,1);
    else
        return streampos(EOF);
}

// A utility function for translated mode counts the newline
// characters in the get area so the file pointer can be moved back
// by an appropriate amount (in_avail()+newlines())
int filebuf::newlines()
{
    char *p = gptr();
    for (int n = 0; p < egptr(); ++p)
        if (*p == '\n') ++n;
    return n;
}

int filebuf::syncin()
{
    if (!do_not_seek) {

// Waste any characters queued in the get area
        int nlc = (mode & ios::translated)? newlines(): 0;
        long offset = in_avail();

        if (gptr_) {
// gptr_ is set if the pushback reserve has been used.
// Dump the pushback and restore the old get buffer.
// Add the pushback to the offset to seek back.
            setg(gptr_, gptr_, egptr_);
            gptr_ = 0;
            nlc += (mode & ios::translated) ?newlines(): 0;
            offset += in_avail();
        }

// This is a weak point. At this stage we have no choice but
// to assume that in translated mode, all newline characters
// in the read buffer were derived from CR/NL pairs in the file.
// This of course may not be the case. There may have been
// isolated newline chars in the file.  This possibility is one
// of the penalties of mimicking Unix in the DOS environment.
        offset += nlc;
        setg(0,0,0);


// Seek back so that it appears that the characters that had
// been queued for read were never read.
        if ( offset > 0 ) {
            long rv = lseek(file, -offset, SEEK_CUR);
            if (rv == -1L) return EOF;
        }
    } else 
        setg(0,0,0);
}

int filebuf::syncout()
{
    int result = 0;
    if (!(mode & ios::out)) {
        setp(0,0);
        return EOF;
    }
// In append-mode, always seek to the end of the file to write
    long cp, rv;
    if (mode & ios::app) {
        cp = lseek(file, 0L, SEEK_CUR);
        if (cp == -1L)
            return EOF;
        rv = lseek(file, 0L, SEEK_END);
        if (rv == -1L)
            return EOF;
    }
    if (flushbuf() == EOF) return EOF;
    if (mode & ios::app) {
        rv = lseek(file, cp, SEEK_SET);
        if (rv == -1L)
            return EOF;
    }
    setp(0,0);
    return 0;
}

// Make the external character stream and the streambuf consistent
// with each other. This means:
// 1. If there are characters in the get area, back up the file pointer
//    by the number of characters which remain unread in the get area.
//    This is only done if the device supports seeks.
// 2. If there are characters in the put area, write them to the
//    file. This leaves the file pointer positioned immediately after
//    the characters just written.
// 3. Zero out both get and put pointer sets (setg(0,0,0), setp(0,0))
// 4. Return EOF if there's an error, otherwise return some other
//    value.

int filebuf::sync()
{
    if ( file == EOF )
        return EOF;
    int rv;
    if (gptr())
        rv = syncin();
    if (rv == EOF) return EOF;
    if (out_waiting())
        rv = syncout();
    return rv;
}
