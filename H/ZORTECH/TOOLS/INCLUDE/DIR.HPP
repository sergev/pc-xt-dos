#ifndef __DIR_HPP
#define __DIR_HPP
#include <dos.h>
#include <string.h>
#include <errno.h>
#include <tools.hpp>
#include <dlist.hpp>


// The FileInfo structure is equivalent to the 'user' portion of
// struct FIND (defined in dos.h).  The directory class just
// consists of a zGDList(FileInfo) with some access functions.
struct FileInfo {
    char attribute;
    unsigned short time,date;
    unsigned long size;
    char name[13];
};

declare(zGDList,FileInfo);

class zDirectory {
public:

// The constructor takes two arguments.  The first specifies a mask to
// restrict the files included in the list, and defaulted to all.  The
// second restricts files based on the file attribute.
    zDirectory(char *spec = "*.*", int attr = 0);

// The interface to the zDirectory class consists of four functions
// to "move" through the directory,  and another to give the count of
// files in the list.  The next and prev functions have no effect if
// they are blocked by the end or the start of the list.
    FileInfo *first()
    {
        int rv = d.start(); return rv? 0: d();
    }
    FileInfo *last()
    {
        int rv = d.end(); return rv? 0: d();
    }
    FileInfo *next()
    {
        int rv = ++d; return rv? 0: d();
    }
    FileInfo *prev()
    {
        int rv = --d; return rv? 0: d();
    }
    int count() { return _count; }

    int error() { int e = err; err = 0; return e; }
private:
    zGDList(FileInfo) d;
    int _count;
    int err;
};
#endif
