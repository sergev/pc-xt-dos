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

// Flush bytes in the put area to the file.
//
// Note that a filebuf is always in one of two states, either getting
// or putting.  The 'other' set of pointers are zeroed to force an
// overflow if getting changes to putting, or an underflow if putting
// changes to getting.  This is the mechanism that essentially 'ties'
// the get and put pointers together in a filebuf, since there is
// no mechanism for this in the underlying streambuf.
//
// It follows that code which switches frequently between getting
// and putting will be inefficient.
int filebuf::overflow(int c)
{
    if (!(mode & ios::out))
        return EOF;
// If there is no buffer, try allocate one
    if (!base()) {
        if (allocate() == EOF) return EOF;
        setg(0,0,0);    // No gets without underflow
    } else {
// Otherwise, flush any characters buffered for write. This
// throws away the contents of the get buffer as if they
// were never fetched from the file, it also zeroes get and
// put area pointers.
        if (in_avail()) {
            if (syncin() == EOF)
                return EOF;
        }
        if (out_waiting()) {
            if (syncout() == EOF)
                return EOF;
        }
    }
// Set up the whole buffer for put
    if (!unbuffered()) {
        int bl = blen();
        setp(base(), base()+((mode & ios::translated)? bl/2: bl));
    }

// Now if not unbuffered, the overflow character can just be
// entered into the put buffer, otherwise it must be written
// directly with translation if required.
    if ( c != EOF ) {
        if ( pptr() < epptr() ) {
            *pptr() = c;
            pbump(1);
        }  else {
            char b = c; // for portability
            long cp, rv;
            if (mode & ios::app) {
                cp = lseek(file, 0L, SEEK_CUR);
                if (cp == -1L)
                    return EOF;
                rv = lseek(file, 0L, SEEK_END);
                if (rv == -1L)
                    return EOF;
            }
            if ((mode & ios::translated) && c == '\n') {
                if(::write(file,"\r\n",2) != 2)
                    return EOF;
            } else {
                if (::write(file, &b, 1) != 1)
                    return EOF;
            }
            if (mode & ios::app) {
                rv = lseek(file, cp, SEEK_SET);
                if (rv == -1L)
                    return EOF;
            }
        }
    }
    return 0;
}

// Empty the buffer to the file. In the translated case this means first
// translating all newline characters to CR/LF pairs, then writing
// this extended length to the file.
int filebuf::flushbuf()
{
    int n;
    char *q;

// The put buffer always lies at the beginning of the buffer area.
// In translating, it gets copied to the other end, thus obliterating
// anything in the get buffer, which had therefore better have been
// dealt with first! This should always be the case since filebuf
// is one track minded!
    if (mode & ios::translated) {
        char *p = pptr()-1;
        q = ebuf();
        for (; p >= pbase();) {
            *--q = *p--;
            if (*q == '\n')
                *--q = '\r';
        }
        n = ebuf()-q;
    } else {
        q = pbase();
        n = pptr() - q;
    }
    long rv, cp;
    if (mode & ios::app) {
        cp = lseek(file, 0L, SEEK_CUR);
        if (cp == -1L)
            return EOF;
        rv = lseek(file, 0L, SEEK_END);
        if (rv == -1L)
            return EOF;
    }

    if (::write(file,q,n) < n) return EOF;

    if (mode & ios::app) {
        rv = lseek(file, cp, SEEK_SET);
        if (rv == -1L)
            return EOF;
    }
    return 0;
}
