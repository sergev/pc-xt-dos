#ifndef DYNARRHPP
#define DYNARRHPP
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>

enum DynArr_errors {
    DYNARR_OK,
    DYNARR_OUTBOUNDS = 80
};

class zDynamicArray {
public:

// The constructor makes provision for a specified size array of elements
// of specified size. Neither needs to be given, and both default to one.
// The array will be resized - memory permitting - when an attempt is made
// to access an array element outside the current range.
    zDynamicArray(unsigned n = 1, unsigned elemsize = 1);
    ~zDynamicArray() { delete body; delete dummy; }

// If extension of the array fails, the access operator will return
// a reference to a dummy element whose value is indeterminate, but
// which can safely be modified without doing any damage.
    char *access(unsigned);

// The address function returns the current address of the actual
// storage which is in use.
    char *address() const { return body; }

// The size function reports the current number of elements
// which the array can hold.
    unsigned size() const { return elems; }
    int error() { int e = err; err = 0; return e; }
private:
    int resize(unsigned);
    char *dummy;
    unsigned elems;
    unsigned es;
    char *body;
    INT16 err;
};

#define zGDynamicArray(type) name2(type,zGDynamicArray)

#define zGDynamicArraydeclare(type)\
struct zGDynamicArray(type): zDynamicArray {\
    zGDynamicArray(type)(unsigned n = 1)\
        : (n, sizeof(type)) {}\
    type& operator[](unsigned i)\
    { return *((type *) zDynamicArray::access(i)); }\
    type *operator&(void) const\
    { return (type *) zDynamicArray::address(); }\
    unsigned size(void) const\
    { return zDynamicArray::size(); }\
private:\
    zGDynamicArray(type)(const zGDynamicArray(type) &);\
    zGDynamicArray(type) &operator=(const zGDynamicArray(type) &);\
}
#endif
