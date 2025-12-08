#include <assert.h>
#include <dos.h>
#include <errno.h>
#include <bucket.hpp>

// Create a new zBucketFile.
zBucketFile::zBucketFile(int ts, size_t ds, INT16 ks, const char *name)
{
    char buf[80];
    bucket = 0;         // safe value

// Form the filename,
    strcpy(buf,name);
    strcat(buf,".bkt");

    if (!access(buf,F_OK)) {    // file exists
        err = BUCKET_CREATE;
        return;
    }
// Work out the actual size of the buckets,
    bucketsize = ds*BUCKETSIZE;
// then allocate a block of memory big enough to hold one.
    bucket = new char[bucketsize];
    if (!bucket) {
        err = ENOMEM;
        return;
    }

// Note the parameters
    tabsize = ts;
    datasize = ds;
    keysize = ks+1;
    traversing = onkey = 0;


// then create the file.
    fd = creat(buf,0644);
    if (fd == -1) {
        err = BUCKET_CREATE;
        return;
    }
    mods = 1;   // is created read/write

// Now fill in the file statistics in the bucket buffer,
    int *p = (int *) bucket;
    *p++ = ts; *p++ = ds; *p = ks;

// and write them as the file header.
    int rv = write(fd,bucket,3*sizeof(int));
    if (rv == -1) {
        err = BUCKET_WRITE;
        return;
    }

// Now zero the buffer and write a blank file
    memset(bucket,0,bucketsize);
    for (int i = 0; i < ts; ++i) {
        rv = write(fd,bucket,bucketsize);
        if (rv == -1) {
            err = BUCKET_WRITE;
            return;
        }
    }
    err = 0;
}

// Open an existing zBucketFile - if mode != 0 the file is
// opened for writing
zBucketFile::zBucketFile(const char *name, INT16 mode)
{
    bucket = 0;
    mods = mode;
    traversing = onkey = 0;
    int buf[3];

// Get the file opened
    char nbuf[80];
    strcpy(nbuf,name);
    strcat(nbuf,".bkt");
    fd = open(nbuf,mods? O_RDWR: O_RDONLY);
    if (fd == -1) {
        err = BUCKET_OPEN;
        return;
    }

// Read the files vital statistics
    read(fd,buf,3*sizeof(int));
    tabsize = buf[0];
    datasize = buf[1];
    keysize = buf[2];

// Allocate a bucket
    bucketsize = datasize*BUCKETSIZE;
    bucket = new char[bucketsize];
    if (!bucket) {
        err = BUCKET_NOMEMORY;
        return;
    }
    err = 0;
}

zBucketFile::~zBucketFile()
{
    delete bucket;
    close(fd);
}

unsigned zBucketFile::hash(char *s)
{
    unsigned t = 0;
    while (*s) t = t << 1 ^ *s++;
    t %= tabsize;
    return t;
}

// Add puts a new record into the zBucketFile. Remember that
// the key is a string, and should be at the start of the data.
int zBucketFile::add(void *a, int *pcp)
{
    traversing = onkey = 0;
    char temp[4];
    char *p, *q = (char *) a;

// Work out which bucket this key belongs in
    long h = hash(q);
    long offset;
    for (int i = 0; i < MAXPROBES; ++i) {

// Read the relevant bucket
        offset = h*bucketsize+3*sizeof(int);
        lseek(fd,offset,0);
        if (read(fd,bucket,bucketsize) != bucketsize)
            return -1;

// Now scan it for an empty slot, or the proposed key value
        p = bucket;
        for (slotno = 0; slotno < BUCKETSIZE; ++slotno) {
            if (!*p) goto slot; // empty slot

            if (!strcmp(q,p))
                return BUCKET_DUPKEY;
                                // key already there
            p += datasize;      // next slot
        }

// No space in that slot, so do a quadratic re-hash to choose an
// alternative bucket.
        h = h+2*i+1;
    }
slot:

// Check that the file is not too full
    if (i < MAXPROBES) {    // found empty slot
// Move the new data into the slot, and write it
        memmove(p,a,datasize);
        lseek(fd,offset,0);
        write(fd,bucket,bucketsize);
        if (pcp)
            *pcp = i+1; // note how many probes
        return 0;
    }
    return BUCKET_OVERLOADED;  // file too full
}

// Seek looks for the data corresponding to a key at the
// the location pointed to by a.
int zBucketFile::seek(void *a)
{
    traversing = 0;
    char *p , *q = (char *) a;

// Generally as add
    long h = hash(q);
    long offset;
    for (int i = 0; i < MAXPROBES; ++i) {
        offset = h*bucketsize+3*sizeof(int);
        lseek(fd,offset,0);
        if (read(fd,bucket,bucketsize) != bucketsize)
            return -1;
        p = bucket;
        for (slotno = 0; slotno < BUCKETSIZE; ++slotno) {

// If we come to an empty slot then this key is not present,
            if (!*p) return BUCKET_NOTFOUND;

// otherwise check for a match
            if (!strcmp(q,p)) {

// Got a match - adjust the file pointer ready for an update, if th
//. file was opened for writing
                if (mods) {
                    lseek(fd,offset,0);
                    onkey = 1;
                }

// Shift the data into the user area.
                memmove(q+keysize,p+keysize,datasize-keysize);
                return 0;   // found it
            }
            p += datasize;
        }
        h = h+2*i+1;
    }
    return BUCKET_NOTFOUND;    // nowhere else to look
}

// Update is used after a successful seek to update
// an existing item.
int zBucketFile::update(void *a)
{
    if (!mods)
        return BUCKET_RDONLY;
    if (!onkey)
        return BUCKET_NOCUR;
    onkey = 0;
    char *p = (char *) a;
    p += keysize;
    char *q = bucket+slotno*datasize+keysize;
    memmove(q,p,datasize-keysize);
    int rv = write(fd,bucket,bucketsize);
    if (rv != bucketsize)
        return BUCKET_WRITE;
    return 0;
}

void *zBucketFile::traverse(int reset)
{
    if (!reset && !traversing)
        return 0;
    if (reset) {
        curbkt = 0;
        slotno = 0;
        lseek(fd,3L*sizeof(int),0);
        read(fd,bucket,bucketsize);
        traversing = 1;
    }
    char *p;
    for (;;) {
        p = bucket + datasize*slotno;
        if (!*p || slotno >= BUCKETSIZE) {
            slotno = 0;
            ++curbkt;
            if (curbkt >= tabsize) {
                traversing = 0;
                return 0;
            }
            lseek(fd,1L*curbkt*bucketsize+3*sizeof(int),0);
            read(fd,bucket,bucketsize);
        } else {
            ++slotno;
            break;
        }
    }
    return p;
}
