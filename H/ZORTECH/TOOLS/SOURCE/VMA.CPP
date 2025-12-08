#include <assert.h>
#include <dos.h>
#include <vma.hpp>

typedef char* cptr;

zVMA::zVMA(long ne, unsigned pb, unsigned rb,
        const char *fname, INT16 create,
        INITFUNC ei, unsigned elemsize)
{
    mbd = new MemBlockData[rb];
    // make list of housekeeping structures one for each resident block
    if (!mbd) {
allerr:
        err = ENOMEM;
        return;
    }
// Mark all blocks as not in use
    for (int i = 0; i < rb; ++i)
        mbd[i].flags = 0;
    rblocks = rb;

// Allocate an array of data block pointers
    bl = new cptr[rb];
    if (!bl) goto allerr;

// Then allocate the actual data blocks,
    for (i = 0; i < rb; ++i) {
// and fill in pointers to them
        bl[i] = new  char[elemsize*pb];
        if (!bl[i]) goto allerr;
    }

// Allocate space for a copy of the filename,
    fn = new char[strlen(fname)+1];
    if (!fn) goto allerr;

// and remember the name for flush
    strcpy(fn,fname);

// Reserve a safe place to refer to if file errors occur
    dummy = new char[elemsize];
    if (!dummy) goto allerr;

    perblock = pb;
    es = elemsize;
// Work the block size out now, it's needed often
    blocksize = perblock*es;
    nelems = ne;

// Round up and allow for a tail mark
    long nblocks = nelems/perblock+2;

// Create or open as requested
    if (create) {
        char status = '\0';
        if ((fd = creat(fname,0644)) == -1) {
            err = VMA_CREATE;
            return;
        }
        for (long nb = 0; nb < nblocks; ++nb) {
// write status marks, a zero byte before each block
            lseek(fd,nb*(blocksize+1),0);
            if (write(fd,&status,1) != 1) {
                err = VMA_WRITE;
                return;
            }
        }
// Close the file, then reopen it to secure it in its initial state
        if (close(fd) == -1) {
            err = VMA_FLUSH;
            return;
        }
        fd = open(fname,O_RDWR);

// Otherwise just open the file
    } else {
        if ((fd = open(fname,2)) == -1) {
            err = VMA_OPEN;
            return;
        }
    }
    elem_init = ei;
    err = 0;
}

char *zVMA::access(long idx, INT16 must_update)
{
    int free, i, t, oldest;

// Scan the resident blocks
    for (free = -1, i = 0, t = 0; i < rblocks; ++i) {
// noting the least recently used block,
        if (mbd[i].asu > t) {
            t = mbd[i].asu;
            oldest = i;
        }
// and checking if there is a block which is not in use.
        if (!(mbd[i].flags & VM_INUSE)) {
            free = i;       // keep a note of an available block
            continue;
        }

// The target element may be in one of the blocks
        if (mbd[i].first_element <= idx
            && idx < mbd[i].first_element+perblock)
            break;
    }
    if (i < rblocks) {
// Element is already in memory, so age everything else.
        age(i);
        if (must_update)
// We will be returning a reference so the element may be altered
            mbd[i].flags |= VM_DIRTY;
// returneturn a char pointer to the element
        return bl[i]+
            es*(idx-mbd[i].first_element);
    }

// The required block is not in memory - need to read it from the disc

    if (free == -1) {       // there is no unused block
        free = oldest;
// If it is marked dirty the file must be updated.
        if (mbd[free].flags & VM_DIRTY)
            file_put(free,mbd[free].first_element);
    }
// Now get the block we need - if file_get
// returns 0 then block has never been used
// so the initialization function must be
// called for the block, and it must be
// marked dirty.
    if (!file_get(free,idx)) {
// Check for file error and return something safe if so
        if (err) return dummy;
        char *p;
        for (p = bl[free], i = perblock; i--; p += es)
            (* elem_init)(p);
        mbd[free].flags = VM_INUSE | VM_DIRTY;
    }
    mbd[free].asu = 0;          // make it most recently used
    mbd[free].first_element
        = (idx/perblock)*perblock;
    mbd[free].flags = VM_INUSE + must_update;
    age(free);      // age the rest
    return bl[free]+
        es*(idx-mbd[free].first_element);
}

int zVMA::file_get(int block, long idx)
{
    char status;

// Calculate offset taking account of status marks
    long offset = (blocksize+1)*(idx/perblock);

    lseek(fd,offset,0);
// Read status mark
    if (read(fd,&status,1) != 1) {
err1:
        err = VMA_READ;
        return 0;
    }
    if (!status)           // block has never been used
        return 0;
    if (read(fd,(char *) bl[block],blocksize) != blocksize)
        goto err1;         // otherwise we actually read it
    return 1;
}

void zVMA::file_put(int block, long idx)
{
    char status = '\1';
    long offset = (blocksize+1)*(idx/perblock);
    lseek(fd,offset,0);
    if (write(fd,&status,1) != 1) {
err2:
        err = VMA_WRITE;
        return;
    }
    if (write(fd,(char *) bl[block],blocksize)
                    != blocksize)
        goto err2;
// Block is now not dirty any more
    mbd[block].flags &= VM_INUSE;
}

// Update file and make safe
void zVMA::flush()
{
// Scan resident blocks, writing the dirty
// ones to file, and marking them clean.
    for (int i = 0; i < rblocks; ++i)
        if (mbd[i].flags & VM_DIRTY) {
            file_put(i,mbd[i].first_element);
            mbd[i].flags &= VM_INUSE;
        }
    if (close(fd) == -1) goto err3;
    if ((fd = open(fn,2)) == -1) {
err3:
        err = VMA_FLUSH;
    }
}

void zVMA::age(int n)
{
// Scan the housekeeping blocks and
// increment the age of everything except n
    for (int i = 0; i < rblocks; ++i)
        if (i != n)
            ++mbd[i].asu;
}

zVMA::~zVMA()
{
    flush(); close(fd);
    delete fn; delete mbd; delete bl;
}
