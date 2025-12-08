#include <string.h>
#include <dynarr.hpp>

int zDynamicArray::resize(unsigned newsize)
// change the size of the storage block - this will always be an increase
{
    char *p = new char[newsize*es];
    if (!p)
        return -1;                      // signal failure
    memset(p,'\0',newsize*es);          // set all to zero
    memmove(p,body,elems*es);           // then copy in existing elements
    delete body;                        // zap the old storage
    body = p;                           // and point to new
    elems = newsize;                    // adjust the element count
    return 0;
}

zDynamicArray::zDynamicArray(unsigned n,unsigned elemsize)
{
    elems = n;
    es = elemsize;
    dummy = new char[es];       // used on allocation error
    if (!dummy)
        goto err1;
    body = new char[es*elems];  // allocate requested storage (default one)
    if (!body) {
err1:
        err = ENOMEM;
        return;
    }
    memset(body,'\0',es*elems); // initialize storage to zero
    err = 0;
}

char *zDynamicArray::access(unsigned i)
{
    if (i >= elems) {           // requested index outside current storage
        if (resize(i+1)) {      // elems is 1 > index
            err = ENOMEM;
            return dummy;       // give access to something safe
        }
    }
    return body+i*es;           // pointer to appropriate block
}
