#ifndef __GQUEUE_HPP
#define __GQUEUE_HPP
#include <generic.hpp>
#include <tools.hpp>
#include <slist.hpp>

#define zGQueue(type) name2(zGQueue,type)

#define zGQueuedeclare(type)\
struct zGQueue(type): zSList {\
    zGQueue(type)(int p = 0) : (p) {}\
    zGQueue(type)(type *a, int p = 0) : (a,p) {}\
    int queue(type *a) { return zSList::append(a); }\
    type *dequeue() { return (type *) zSList::get(); }\
    int error() { return zSList::error(); }\
private:\
    zGQueue(type)(const zGQueue(type) &);\
    zGQueue(type) &operator=(const zGQueue(type) &);\
}
#endif
