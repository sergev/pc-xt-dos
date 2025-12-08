#ifndef __GSTACK_HPP
#define __GSTACK_HPP
#include <generic.hpp>
#include <tools.hpp>
#include <slist.hpp>

#define zGStack(type) name2(zGStack,type)

#define zGStackdeclare(type)\
struct zGStack(type): zSList {\
    zGStack(type)(int p = 0) : (p) {}\
    zGStack(type)(type *a, int p = 0) : (a,p) {}\
    int push(type *a) { return zSList::insert(a); }\
    type *pop() { return (type *) zSList::get(); }\
    int error() { return zSList::error(); }\
private:\
    zGStack(type)(const zGStack(type) &);\
    zGStack(type) &operator=(const zGStack(type) &);\
}
#endif
