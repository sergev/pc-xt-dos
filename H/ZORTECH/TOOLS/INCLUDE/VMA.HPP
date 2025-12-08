#ifndef VMAHPP
#define VMAHPP
#include <string.h>
#include <io.h>
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>

const int VM_DIRTY = 1;   // miscellaneous flag bits
const int VM_INUSE = 2;

enum VMA_errors {
    VMA_OK,
    VMA_CREATE = 300,
    VMA_WRITE,
    VMA_FLUSH,
    VMA_OPEN,
    VMA_READ
};

typedef void (* INITFUNC)(void *);

// A private data type is defined as a friend of class zVMA to
// look after the in-memory block housekeeping information.
class MemBlockData {
    friend class zVMA;

    long first_element;
                    // element no of first element this block
    unsigned asu;
                    // accesses since used
    unsigned flags;
                    // indicating block in use and write required
};

class zVMA {
protected:

// The constructor takes a long argument specifying the number of elements
// in the array.  Two unsigned arguments then specify the number of data
// objects to be transferred per disk access (per block),  and the number
// of blocks of the same size which are to be held in memory.  Between
// these two there is some scope for optimization for particular applications.
//
// The next argument specifies the file name,  and the following
// INT16 argument determines whether the constructor will attempt to
// create and initialize a file of the required size, or simply attempt
// to open an existing one.
//
// The function pointer argument specifies a function to be 
// called to initialize a hitherto unused memory block, and the final
// argument gives the element size.
    zVMA(long ne, unsigned pb, unsigned rb,
        const char *fn, INT16 cf, INITFUNC ei,
        unsigned elemsize);

// Dual purpose access function
    char *access(long, INT16);
// Flush everything to file
    void flush(void);

    int error() { int e = err; err = 0; return e; }
    ~zVMA();
private:
    int file_get(int, long);      // read a block
    void file_put(int, long);     // write a block
    void age(int);          // update the asu fields
    void (* elem_init)(void *);  // function to initialize an element

    INT16 err;
    char *fn;               // file name
    int fd;                 // associated file descriptor
    long nelems;            // total no of elements
    unsigned perblock;      // no of items read or written at a time
    int rblocks;            // no of blocks resident in memory
    unsigned blocksize;     // bytes per block
    unsigned es;            // element size
    MemBlockData *mbd;      // pointer to memory block data list
    char **bl;              // pointer to memory block pointer list
    char *dummy;            // a safe sacrificial element
};

#define zGVMA(type) name2(type,zGVMA)

#define zGVMAdeclare(type)\
struct zGVMA(type): zVMA {\
    zGVMA(type)(long te,unsigned pb,\
            unsigned bs,const char *fn,\
            int cf, INITFUNC ei) :\
    (te,pb,bs,fn,cf,ei,sizeof(type)) {}\
    type& operator[](long i)\
        { return *((type *) zVMA::access(i,1)); }\
    type operator()(long i)\
        { return *((type *) zVMA::access(i,0)); }\
    int error() { return zVMA::error(); }\
private:\
    zGVMA(type)(const zGVMA(type) &);\
    zGVMA(type) &operator=(const zGVMA(type) &);\
}
#endif
