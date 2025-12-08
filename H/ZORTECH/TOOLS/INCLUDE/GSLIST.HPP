#ifndef __GSLIST_HPP
#define __GSLIST_HPP
#include <generic.hpp>
#include <tools.hpp>
#include <slist.hpp>

#define zGSList(type) name2(zGSList,type)
#define zGSListIterator(type) name2(zGSListIterator,type)

// Note that the copy constructor and assignment operator are
// declared private. Furthermore they are not implemented, i.e.
// these operations are not supported for zGSList objects

#define zGSListdeclare(type)\
struct zGSList(type): zSList {\
    int insert(type *a) { return zSList::insert(a); }\
    int append(type *a) { return zSList::append(a); }\
    type *get() { return (type *) zSList::get(); }\
    zGSList(type)(int p = 0) : (p) {}\
    zGSList(type)(type *a, int p = 0) :\
        ((void *) a,p) {}\
private:\
    zGSList(type)(const zGSList(type) &);\
    zGSList(type) &operator=(const zGSList(type) &);\
}

#define zGSListIteratordeclare(type)\
struct zGSListIterator(type) : zSListIterator {\
    zGSListIterator(type)(zGSList(type)& s) :\
        (s) {}\
    type *operator()()\
    { return (type *) zSListIterator::operator()(); }\
}
#endif
