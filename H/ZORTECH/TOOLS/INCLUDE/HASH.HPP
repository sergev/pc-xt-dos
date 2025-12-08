#ifndef __HASH_HPP
#define __HASH_HPP
#include <string.h>
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>
#include <slist.hpp>

enum Hash_errors {
    HASH_OK = 0,
    HASH_BADARG = 120,
    HASH_DUP,
    HASH_NOSUCH
};

// As in the case of the binary tree class, we define function pointer
// types to allow the user to specify functions for comparing objects,
// visiting objects in the table, and, in this case, calculating a
// hash number.  The hash function can make use of the function
// zAlphaHash which is include in the library if this is helpful.
// Its second argument is the table size.  The return value from
// the hash function should be modulo this table size value. zAlphaHash
// is designed to take table sizes which are a power of two.
typedef unsigned (* HASHFUNC)(void *, unsigned);
typedef int (* MATCHING)(void *, void *);
typedef void (* VISIT)(void *);
unsigned zAlphaHash(const char *, unsigned);

class zHashTable {

    friend void zHashTable_report(zHashTable&,
        const char *(*xf)(void *));

protected:

// There is only one constructor. The first argument is required, and
// must point to the function which is to be used to compare the objects
// to be stored in the table.  In the case of the hash table all this
// needs to do is return zero if the objects are equal, nonzero otherwise.
// The second integer argument determines the table size and must also
// be supplied.  The third argument is a pointer to the user specified
// hash function.  The fourth argument is defaulted to zero,  and is used
// to control the cleanup behaviour of the hash table.
// If a non-zero value is supplied, the destructor will delete
// the objects in the table as well as the table nodes themselves.
// The next argument is a pointer to whatever function is to be executed
// when the tree nodes are visited via the inorder function. This
// is defaulted to zero so that setting it can be deferred.
    zHashTable(MATCHING, int tablesize,
        HASHFUNC, int clean = 0, VISIT  = 0);

// Add an item to the table.
    int insert(void *);

// Remove an item, and return a pointer to it.
    void *remove(void *);

// See if an item is in the table.
    void* seek(void *);

// The action required when a node is visited can be adjusted during the
// lifetime of the table.
    void set_process(VISIT v) { visit = v; }

// The inorder function does a pass through the table,  at each node
// the visit function is called.  It should be appreciated however
// that the order in which items will be visited is rather
// arbitrary.  It is determined by the hash function, and that does
// the best it can to distribute objects randomly.
    void inorder();
    int error() { int e = err; err = 0; return e; }
    ~zHashTable() { cleanup(); }
private:
    void cleanup();
    zSList **table;
    int tabsize;
    unsigned char err;
    unsigned char purging;
    HASHFUNC hashfunc;
    MATCHING matching;
    VISIT visit;
};

#define zGHSearch(type) name2(zGHSearch,type)

#define zGHSearchdeclare(type)\
struct zGHSearch(type) : zHashTable {\
    zGHSearch(type)(MATCHING mf, int s,\
        HASHFUNC f, int p, VISIT v)\
          : (mf,s,f,p,v) {}\
    int insert(type *d)\
        { return zHashTable::insert(d); }\
    type *remove(type *d)\
        { return zHashTable::remove(d); }\
    type *seek(type *d)\
        { return zHashTable::seek(d); }\
    void set_process(VISIT v)\
        { zHashTable::set_process(v); }\
    void inorder() { zHashTable::inorder(); }\
    int error() { return zHashTable::error(); }\
private:\
    zGHSearch(type)(const zGHSearch(type) &);\
    zGHSearch(type) &operator=(const zGHSearch(type) &);\
}
#endif
