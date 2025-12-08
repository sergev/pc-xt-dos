// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991 to allow translated (DOS style) IO
// Copyright Zortech 1990-1991. All Rights Reserved.

#define _MT	1		// make sure we are thread-aware

#include <disp.h>

#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <fstream.hpp>

// The default buffer size is set to be an "efficient" size
// for a disk block I/O. This value affects the efficiency
// of I/O, and the amount of memory used by the filebuf.

// Default buffer sizes
#if M_UNIX || M_XENIX
const int _dbs = 8192;   /* BSD filesystem */
#elif __I86__ >= 3
const int _dbs = 0x4000;	/* 386	  */
#else
const int _dbs = 1024;		/* MS-DOS */
#endif

// The default constructor.
filebuf::filebuf()
    :   file(EOF),
        mode(0),
        do_not_seek(0),
        gptr_(0),
        egptr_(0),
        own_file_descriptor(0)
{}

// Constructs a filebuf for the open file attached to the argument
// file descriptor.
filebuf::filebuf(int descriptor, int io_mode)
    :   file(descriptor),
        mode(io_mode),
        do_not_seek(isatty(descriptor)),
        gptr_(0),
        egptr_(0),
        own_file_descriptor(0)
{
// The file may not have been opened with O_APPEND
    if (mode & ios::ate)
        lseek(file, 0L, SEEK_END);
}

// Constructs a filebuf for the open file attached to
// "file_descriptor", and sets the buffer to "memory", which is of
// "length" bytes in size. If memory is 0 or length is <= 0,
// it is taken as a request that the file be unbuffered.

filebuf::filebuf(int descriptor, char *memory, int length, int io_mode)
    :   streambuf(memory, length),
        file(descriptor),
        mode(io_mode),
        do_not_seek(isatty(descriptor)),
        gptr_(0),
        egptr_(0),
        own_file_descriptor(0)
{
    buffer_setup();
// The file may not have been opened with O_APPEND
    if (mode & ios::ate)
        lseek(file, 0L, SEEK_END);
}

filebuf::~filebuf()
{
    sync();
    if (own_file_descriptor)
        close();
}

filebuf *filebuf::attach(int file_descriptor, int io_mode)
{
    if (file != EOF)
        return 0;
    
    file = file_descriptor;
    if (file == EOF)
        return 0;
    mode = io_mode;
    if (isatty(file) || lseek(file, 0L, SEEK_CUR) < 0)
        do_not_seek = 1;
    else {
// The file may not have been opened with O_APPEND
        if (mode & ios::ate)
            lseek(file, 0L, SEEK_END);
    }

    return this;
}

// Flushes output, closes the file, and detaches the file from this
// filebuf. Clears the error state unless there is an error flushing
// the output. Will always close the file and detach it from the
// filebuf, even if there are errors.
filebuf *filebuf::close()
{
    int result = sync();

    int rv = (file == EOF)? EOF: ::close(file);
    file = EOF;

    if ( result == EOF || rv)
        return 0;
    else
        return this;
}

int filebuf::doallocate()
{
    int bl = (mode & ios::translated)? _dbs*2: _dbs;
    char *buffer = new char[bl];

    if (!buffer)
        return EOF;
    setb(buffer, buffer+bl,1);
    return 1;
}

// Opens the file "name", and connects it to this filebuf.
// Mode is a bit-mask containing one or more of the values of
// enum open_mode:
//  ios::in     Open for reading.
//  ios::out    Open for writing.
//  ios::ate    Position to the end-of-file.
//  ios::app    Position to the end-of-file before each write.
//          ios::app implies ios::out .
//  ios::trunc  Truncate the file on open.
//  ios::nocreate   Do not attempt to create the file if it
//          does not exist.
//  ios::noreplace  Cause the open to fail if the file exists.
//  ios::translate  Make CR/LF pairs into newlines on input
//          and vice versa on output
//

filebuf *filebuf::open(const char *name, int io_mode, int protection)
{
    if (file != EOF) {

// Existing file needs close() first
        errno = EINVAL;
        return 0;
    }
        
    int open_mode = 0;
    mode = io_mode;

    if (mode & (ios::out|ios::app)) {
        if (mode & ios::in)
            open_mode |= O_RDWR;
        else
            open_mode |= O_WRONLY;
    }
    else if (mode & ios::in)
        open_mode |= O_RDONLY;
    else {

// Must have one or both of ios::in and ios::out set
        errno = EINVAL;
        return 0;
    }

    if (mode & ios::ate)
        open_mode |= O_APPEND;
    if (mode & ios::trunc
    || (mode & ios::out && !(mode & (ios::in|ios::ate|ios::app)) ) )
        open_mode |= O_TRUNC;
    if (!(mode & ios::nocreate))
        open_mode |= O_CREAT;
    if (mode & ios::noreplace)
        open_mode |= O_EXCL;

    if ((file = ::open(name, open_mode, protection)) < 0)
        return 0;
// ::open does seek to end if ios::ate, seek to
// end is done for each write if ios::app, and after such
// append writes the file pointer is restored to its former
// position. So in app mode, the get and put pointers are
// not tied.

    own_file_descriptor = 1;

    if (isatty(file) || lseek(file, 0, SEEK_CUR) < 0)
        do_not_seek = 1;
    return this;
}

// Request that this streambuf use the argument memory buffer.
// A null memory argument, or a zero or negative length argument,
// is taken as a request that this streambuf be unbuffered.
// If the request to set the buffer is honored, return this, otherwise
// return zero.

streambuf *filebuf::setbuf(char *memory, int length)
{
    if (unbuffered() || base())
// If it is already working unbuffered, or has a proper
// buffer, decline the offer.  The one offered may be an
// unsuitable size
        return 0;
    if (streambuf::setbuf(memory, length)) {
        buffer_setup();
        return this;
    } else
        return 0;
}

void filebuf::buffer_setup()
{
// Use both chars of unbuf if translation necessary
    if (unbuffered() && (mode & ios::translated))
        setb(base(), base()+2, 0);
// Zero the other pointers
    setg(0, 0, 0);
    setp(0, 0);
}
