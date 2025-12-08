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

// Get more bytes for reading.  See the comment before filebuf::overflow.
int filebuf::underflow()
{
    if (!(mode & ios::in))
        return EOF;
// If we are using the pushback reserve, go back to using the
// regular get buffer. There should always be characters in it.
    if (gptr_) {
        setg(gptr_, gptr_, egptr_);
        gptr_ = 0;
        return (unsigned char) *gptr();
    }

// If there is no buffer, try to allocate one
    if (!base()) {
        if ((allocate()) == EOF) return EOF;
        setp(0,0);  // No puts without overflow
    } else {
// The file pointer needs to be adjusted if any characters have
// been put since last time the get buffer was filled.
        if (out_waiting()) {
            if (syncout() == EOF)
                return EOF;
        }
    }
// Now go for some more of the file
    int result = fillbuf();
    if ( result != EOF) {

// Set the get buffer to the characters just read
        setg(base(), base(), base()+result);
        return (unsigned char) *gptr();
    } else {

// Get here if there's been an I/O error or EOF while reading.
        setg(0, 0, 0);
        return EOF;
    }
}

// Try to fill the buffer from the file. In the DOS case this
// requires reading a buffer full, then stripping out CR/LF pairs
// and substituting LF, and returning the number of translated
// characters. fillbuf() uses the entire buffer area, so any chars
// in the put area need to have been dealt with first.
int filebuf::fillbuf()
{
    int trans = (mode & ios::translated)? 1: 0;
    char *p = base(), *q;
    int required = blen()-trans;

    int t = ::read(file,p,required);
    if (t <= 0) return EOF;   // no more characters

    if (trans) {
        q = p+(t-1);
        if (*q++ =='\r') {
// If there is an isolated CR try and make a pair
            if (::read(file,q,1) == 1) {
                if (*q != '\n') {
                    lseek(file,-1L,SEEK_CUR);
// if it doesn't work then step the file back.  This should only
// happen on the infrequent occasions when isolated return characters
// just happen to lie at the limit of the buffer area.
                } else
// usually it will be one of a pair
                    ++t;
            }
        }
    }

// If not translated that is all
    else
        return t;

// Otherwise do the translation
    q = p;
    for (; t--;) {
        if (t > 0 && *((short *) p) == 0x0a0d) {
            ++p;    // skip the '\r'
            --t;
        }
        *q++ = *p++;
    }
    return q-base();  // the translated length
}
