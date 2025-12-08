#ifndef VMSHPP
#define VMSHPP
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <generic.hpp>
#include <tools.hpp>

const int VM_DIRTY = 1;
const int VM_INUSE = 2;

enum VMS_errors {
    VMS_OK = 0,
    VMS_CREATE = 320,
    VMS_OPEN_INDEX,
    VMS_READ_INDEX,
    VMS_OPEN,
    VMS_WRITE_INDEX,
    VMS_OUTBOUNDS,
    VMS_INCONSISTENT,
    VMS_READ,
    VMS_WRITE,
    VMS_FLUSH
};

typedef void (* INITFUNC)(void *);

//. We need various private data types to support the sparse virtual
//. array class. These are defined as a friends of class zVMS.
class IndexItem {
    friend class zVMS;

public:
    IndexItem(long l = 0L, long a = 0L, unsigned s = -1)
        { logical = l; actual = a; slot = s; }
private:
    IndexItem *next;
    long logical,actual;
    unsigned slot;
};

class Residence {
    friend class zVMS;

    unsigned asu;           // accesses since used
    unsigned flags;         // VM_INUSE & VM_DIRTY
    long ind;               // logical index number of this block
    char it;                // placemarker for data
};

typedef Residence *ResidencePtr;

class zVMS {
protected:

// The constructor takes a long argument specifying the maximum number
// of elements in the array.  Two unsigned arguments then specify the
// number of data objects to be resident in memory,  and the number of
// hash chains which are to be maintained. Between these two there is
// some scope for optimization for particular applications.
//
// The next argument specifies the file name,  and the following
// INT16 argument determines whether the constructor will attempt to
// create and initialize a filesystem, or simply attempt to open an
// existing one.
//
// The function pointer argument specifies a function to be
// called to initialize a hitherto unused memory block, and the final
// argument gives the element size.
    zVMS(long max, unsigned ri, unsigned iw,
        const char *fn, INT16 cf,
        INITFUNC ei, unsigned elemsize);

// Dual purpose access function
    char *access(long, INT16);

// Report number of active elements
    long nelems(void) const { return ccount; }
// Flush everything to file
    void flush();

int error() { int e = err; err = 0; return e; }
    ~zVMS() { flush(); fclose(fp);
                    cleanup(); delete fn; delete rp; }
private:
    void insert(IndexItem *);  // put item in index
    IndexItem *lookup(long);   // check index for item
    void cleanup();            // purge the index
    void reindex();            // rebuild index file
    void file_get(int, IndexItem *);      // read a block
    void file_put(int, IndexItem *);      // write a block
    void age(int);             // update the asu fields
    void (* elem_init)(void *);  // function to initialize an element

    INT16 err;
    FILE *fp;                   // data file pointer
    char *fn;                   // file name
    long nmax;                  // largest permissible index
    unsigned ritems;            // no of items resident in memory
    unsigned es;                // element size
    char *empty;                // points at copy of initialized element
    long ccount;                // number in file
    int freeblocks;             // blocks not yet used
    ResidencePtr *rp;           // pointer to resident block pointer array
    IndexItem **table;          // pointer to list of IndexItem chains
    unsigned tabsize;           // no of chains
};

#define zGVMS(type) name2(type,zGVMS)

#define zGVMSdeclare(type)\
struct zGVMS(type): zVMS {\
    zGVMS(type)(long me, unsigned ri,\
                unsigned iw, char *fn,\
        int cf, INITFUNC ei) :\
        (me,ri,iw,fn,cf,ei,sizeof(type)) {}\
    void flush() { zVMS::flush(); }\
    type& operator[](long i)\
        { return *((type *) zVMS::access(i,1)); }\
    type operator()(long i)\
        { return *((type *) zVMS::access(i,0)); }\
    long nelems(void) const { return zVMS::nelems(); }\
    int error() { return zVMS::error(); }\
private:\
    zGVMS(type)(const zGVMS(type) &);\
    zGVMS(type) &operator=(const zGVMS(type) &);\
}
#endif
