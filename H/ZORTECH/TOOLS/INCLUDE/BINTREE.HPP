#ifndef __BINTREE_HPP
#define __BINTREE_HPP
#include <stddef.h>
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>

#if LCODE
#define NULLF 0L
#else
#define NULLF 0
#endif

enum zBinTree_errors {
    BINTREE_OK = 0,
    BINTREE_BADARG = 100,
    BINTREE_DUP,
    BINTREE_NOSUCH
};

enum zBinTree_cleanup {
    PASSIVE,
    PURGE
};

class zBinTreeNode {
    friend class zBinTree;
    zBinTreeNode *left, *right;
    void* body;
};

// In the course of building a tree it will be neccessary to make
// comparisons of whatever objects are to be stored in it.  Since
// we do not know at this stage what these will be, we define a type
// for a pointer to a user supplied function.  A similar typedef
// is also required for the user supplied function which determines
// what is done when a tree node is "visited".
typedef int (* COMPFUNC)(void *, void *);
typedef void (* VISIT)(void *);

class zBinTree {

protected:

// There is only one constructor. The first argument is required, and
// must point to the function which is to be used to compare the objects
// to be stored in the tree.  The second integer argument is defaulted
// to zero,  and is used to control the cleanup behaviour of the tree.
// If a non-zero value is supplied, the tree destructor will delete
// the objects attached to the tree nodes as well as the nodes themselves.
// The final argument is a pointer to whatever function is to be executed
// when the tree nodes are visited via the inorder function. This
// is defaulted to zero so that setting it can be deferred.
    zBinTree(COMPFUNC compare, int = 0,
            VISIT what_to_do = 0);

    int insert(void *);
    void *remove(void *);
    void* seek(void*);

// The action required when a node is visited can be adjusted during the
// lifetime of the tree.
    void set_process(VISIT what_to_do)
        { visit = what_to_do; }

// The inorder function does a pass through the tree,  at each node
// the what_to_do function is called.  This can be used, for instance,
// to print out the contents of the tree in the order determined
// by the comparison function.
    void inorder(zBinTreeNode* n = NULL, int first = 1);
    int error() { int e = err; err = 0; return e; }
    ~zBinTree(void) { cleanup(); }
private:
    void cleanup(zBinTreeNode* n = NULL, int first = 1);
    char err;
    char purging;
    zBinTreeNode* root;
    int (* cf)(void*, void*);
    void (* visit)(void*);
};

#define zGBSearch(type) name2(zGBSearch,type)

#define zGBSearchdeclare(type)\
struct zGBSearch(type) : zBinTree {\
    zGBSearch(type)(int (* compare)(type *,type *),\
        int p = 0, void (*what_to_do)(type *) = NULLF) :\
        (compare, p, what_to_do) {}\
    int insert(type *a)\
        { return zBinTree::insert(a); }\
    type *remove(type *a)\
        { return zBinTree::remove(a); }\
    type *seek(type *a)\
        { return (type *) zBinTree::seek(a); }\
    void set_process(void (*v)(void*))\
        { zBinTree::set_process(v); }\
    void inorder(void) { zBinTree::inorder(); }\
    int error() { return zBinTree::error(); }\
private:\
    zGBSearch(type)(const zGBSearch(type) &);\
    zGBSearch(type) &operator=(const zGBSearch(type) &);\
}
#endif
