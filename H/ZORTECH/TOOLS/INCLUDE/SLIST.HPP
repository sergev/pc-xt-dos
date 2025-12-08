#ifndef __SLIST_HPP
#define __SLIST_HPP
#include <stddef.h>
#include <errno.h>
#include <tools.hpp>

#if LPTR
#define LCAST (long)
#else
#define LCAST
#endif

enum slist_errors {
    SLIST_OK,
    SLIST_EMPTY = 40
};

class zSLink {
    friend class zSList;             // Members must be accessible to zSList
    friend class zSListIterator;     // and to the iterator, but that is all

    zSLink(void *a) {e = a; next = 0; }
    zSLink* next;
    void *e;
};

class zSList {
    friend class zSListIterator;

public:

// There are two constructors for zSList. a default constructor,
// which can be used without arguments,  and one which takes a void*
// argument.  Both have an optional trailing integer argument which
// determines the cleanup behaviour of the class.  If it is nonzero
// (default is zero), then the destructor for class zSList will delete
// the objects attached to the list as well as the zSLink objects
// comprising the list.
    zSList(INT16 purge = 0);
    zSList(void *a, INT16 purge = 0);

// insert attaches an item at the head of the list.
    int insert(void *);

// append adds an item at the list tail.
    int append(void *);

// get removes and returns a pointer to the item at the head of
// the list
    void *get();

    int error()
        { int e = err; err = 0; return e; }
    ~zSList() { cleanup(); }
private:

    zSLink* last;            // last->next is head of list
    char err;
    char purging;
    void cleanup();          // delete nodes and data if purging
};

// An iterator allow us to progress along a list
class zSListIterator {
public:

// The iterator has one constructor which takes a reference to a
// zSList as argument.
    zSListIterator(zSList& s) { cs = &s; ce = 0; }
    void *operator()();
private:
    zSLink* ce;
    zSList* cs;
};
#endif
