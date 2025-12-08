#ifndef __DLIST_HPP
#define __DLIST_HPP
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>

enum DList_errors {
    DLIST_OK,
    DLIST_EMPTY = 60,
    DLIST_ATEND
};

class zDLNode {
    friend class zDList;
    friend class zDLCursor;
    zDLNode *next, *prev;   // pointers to next and previous items in list
    void *body;             // points to whatever is associated with this node
};

class zDList {
    friend class zDLCursor;

public:

// The optional integer argument to the constructors enables purging
// of the items on the list by the list destructor.  If the default
// zero argument is used,  the destructor deletes only the list nodes.
    zDList(INT16 = 0);
    zDList(void* a, INT16 = 0);

// linkin adds a new item to the list after the current item, and
// moves the currency onto the new item.
    int linkin(void *);

// linkout removes the current item from the list, and returns a
// pointer to it.
    void* linkout();

// Four functions provide for moving about the list. fwd and bkwd
// move one item forwards or backwards.  If the currency
// was already at the end or the beginning respectively, then these
// functions have no effect. Functions start and end move the
// currency to either end of the list.
    int fwd();
    int bkwd();
    int start();
    int end();

// The get function removes an item from the list, and returns a
// pointer to it.
    void *get();

// update exchanges the object pointed at by its argument with the
// one already in the list at the current position.  A pointer to
// the displaced object is returned.
    void *update(void *);

// size gets the number of items which are present in the list.
    long size(void) const { return count; }
    int error() { int e = err; err = 0; return e; }
    ~zDList() { cleanup(); }

private:
    void cleanup();
    zDLNode *root;           // root->next is head of list *root is tail
    zDLNode *current;        // remembers current context
    long count;              // how many items in the list
    long lpos;               // where are we now
    unsigned char purge;     // delete objects as well as nodes?
    unsigned char err;
};

// The cursor class reproduces the movement and access capabilities
// of the list currency.
class zDLCursor {
public:

// The constructor takes a reference to a zDList as argument.  The optional
// argument determines the relationship of the cursor currency to the
// zDList currency. If it is nonzero the cursor currency is synchronized
// to the list currency. If zero it is set to the start of the list.
    zDLCursor(zDList&, int = 0);
    int fwd();
    int bkwd();
    int start();
    int end();
    void* get();
    void *update(void*);
    int insync() { return lpos == thislist.lpos; }
    int error() { return thislist.err; }
    
private:
    zDList &thislist;    // list this cursor is riding
    long lpos;          // where are we now
    zDLNode *current;
};
    
#define zGDList(type) name2(zGDList,type)

#if __ZTC__ < 0x215
#define zGDListdeclare(type)\
struct zGDList(type) : zDList {\
    zGDList(type)(int p = 0) : (p) {}\
    zGDList(type)(type *a, int p = 0) : (a,p) {}\
    int linkin(type *a)\
        { return zDList::linkin(a); }\
    int operator+=(int n)\
        { int r; while (n--) r = fwd(); return r; }\
    int operator-=(int n)\
        { int r; while (n--) r = bkwd(); return r; }\
    int operator++() { return fwd(); }\
    int operator--() { return bkwd(); }\
    type *update(type *a)\
        { return (type *) zDList::update(a); }\
    type *operator*()\
        { return (type *) linkout(); }\
    type *operator()() { return (type *) get(); }\
    int start() { return zDList::start(); }\
    int end() { return zDList::end(); }\
    long size() const { return zDList::size(); }\
    int error() { return zDList::error(); }\
private:\
    zGDList(type)(const zGDList(type) &);\
    zGDList(type) &operator=(const zGDList(type) &);\
}
#else
#define zGDListdeclare(type)\
struct zGDList(type) : zDList {\
    zGDList(type)(int p = 0) : (p) {}\
    zGDList(type)(type *a, int p = 0) : (a,p) {}\
    int linkin(type *a)\
        { return zDList::linkin(a); }\
    int operator+=(int n)\
        { int r; while (n--) r = fwd(); return r; }\
    int operator-=(int n)\
        { int r; while (n--) r = bkwd(); return r; }\
    int operator++() { return fwd(); }\
    int operator--() { return bkwd(); }\
    int operator++(int) { return fwd(); }\
    int operator--(int) { return bkwd(); }\
    type *update(type *a)\
        { return (type *) zDList::update(a); }\
    type *operator*()\
        { return (type *) linkout(); }\
    type *operator()() { return (type *) get(); }\
    int start() { return zDList::start(); }\
    int end() { return zDList::end(); }\
    long size() const { return zDList::size(); }\
    int error() { return zDList::error(); }\
private:\
    zGDList(type)(const zGDList(type) &);\
    zGDList(type) &operator=(const zGDList(type) &);\
}
#endif
    
#define zGDLCursor(type) name2(type,zGDLCursor)
#if __ZTC__ < 0x215
#define zGDLCursordeclare(type)\
struct zGDLCursor(type) : zDLCursor {\
    zGDLCursor(type)(zGDList(type)& a, int s = 0)\
                : (a, s) {}\
    int insync() { return zDLCursor::insync(); }\
    int operator+=(int n)\
        { int r; while (n--) r = fwd(); return r; }\
    int operator-=(int n)\
        { int r; while (n--) r = bkwd(); return r; }\
    int operator++() { return fwd(); }\
    int operator--() { return bkwd(); }\
    type *update(type *a)\
        { return (type *) zDLCursor::update(a); }\
    type *operator()() { return (type *) get(); }\
    int start() { return zDLCursor::start(); }\
    int end() { return zDLCursor::end(); }\
    int error() { return zDLCursor::error(); }\
}
#else
#define zGDLCursordeclare(type)\
struct zGDLCursor(type) : zDLCursor {\
    zGDLCursor(type)(zGDList(type)& a, int s = 0)\
                : (a, s) {}\
    int insync() { return zDLCursor::insync(); }\
    int operator+=(int n)\
        { int r; while (n--) r = fwd(); return r; }\
    int operator-=(int n)\
        { int r; while (n--) r = bkwd(); return r; }\
    int operator++() { return fwd(); }\
    int operator--() { return bkwd(); }\
    int operator++(int) { return fwd(); }\
    int operator--(int) { return bkwd(); }\
    type *update(type *a)\
        { return (type *) zDLCursor::update(a); }\
    type *operator()() { return (type *) get(); }\
    int start() { return zDLCursor::start(); }\
    int end() { return zDLCursor::end(); }\
    int error() { return zDLCursor::error(); }\
}
#endif
#endif
