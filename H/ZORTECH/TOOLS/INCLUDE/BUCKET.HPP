#ifndef __BUCKET_HPP
#define __BUCKET_HPP
#include <io.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <generic.hpp>
#include <tools.hpp>

const int BUCKETSIZE = 5;
const int MAXPROBES = 5;

enum bucket_constants {
    BUCKET_OK,
    BUCKET_CREATE = 140,
    BUCKET_WRITE,
    BUCKET_OPEN,
    BUCKET_NOMEMORY,
    BUCKET_DUPKEY,
    BUCKET_OVERLOADED,
    BUCKET_NOTFOUND,
    BUCKET_RDONLY,
    BUCKET_NOCUR
};
    
class zBucketFile {
public:

// The first constructor is the one which is used to create a
// bucket file from scratch. The first argument specifies
// the hash table size,  and it should be a prime number.  The class
// will work if it is not, but performance will be degraded.  The
// second argument specifies the size of the objects to be stored
// in the zBucketFile,  which will be expressed as sizeof
// some type.  The third argument gives the length of the key to
// be used in searching for the item.  It is assumed that this is
// a string,  and is located at the start of the data object (at
// offset zero).  Finally the last, char pointer argument specifies
// a name for the zBucketFile.  The extension "BKT" is added
// automatically by the constructor.
    zBucketFile(int, size_t, INT16, const char *);

// The second constructor opens an existing bucketfile.  The second
// parameter determines whether or not modifications are to be permitted.
// The default is read only.
    zBucketFile(const char *, INT16 = 0);
    ~zBucketFile();

// The add function puts the data pointed at by va into the zBucketFile.
// Remember that the key is a string, and should be at the start of the
// data.  The defaulted second argument is provided solely for performance
// testing.  The integer pointer argument can be used to specify the
// address of an integer variable to receive the number of probes which
// were required to retrieve a key.  If this gets excessive, a bigger
// table size is indicated.
    int add(void *va, int * = 0);

// seek gets the data corresponding to a key into a specified
// memory location.  The key should be written into that location
// before the call to seek.
    int seek(void *);

// The update function is used after a successful seek to modify an
// existing item, providing, of course, that the zBucketFile has been
// opened for writing.
    int update(void *);

// The traverse function allows the contents of a zBucketFile to be
// listed. A call to traverse with a non-zero argument resets to the
// "first" record in the file.  The meaning of first in the case of a
// zBucketFile is pretty arbitrary.  It will depend on the hash algorithm,
// and the order in which keys were inserted.  If you need an ordered
// list you must undertake the sorting.  Subsequent calls to traverse
// with no argument will retrieve the "next" record.  When there are no
// more records traverse will return a null pointer.
    void *traverse(int = 0);

// The usual error function returns the value of the class error flag,
// and resets the flag.
    int error() { int e = err; err = 0; return e; }
private:
    unsigned hash(char *);

    int tabsize;    // should be prime
    size_t datasize;
    INT16 keysize;
    size_t bucketsize;
    int fd;
    char err;
    char onkey;
    int slotno;
    char mods;
    char traversing;
    int curbkt;
    char *bucket;
};

#define zGBucketFile(type) name2(zGBucketFile,type)

#define zGBucketFiledeclare(type)\
struct zGBucketFile(type) : zBucketFile {\
    zGBucketFile(type)(size_t ts, int keylen,\
        const char *fn) :\
        (ts,sizeof(type),keylen,fn) {}\
    zGBucketFile(type)(const char *fn, int om = 0)\
        : (fn,om) {}\
    int add(type *d, int *ip = 0)\
        { return zBucketFile::add(d,ip); }\
    int seek(type *d)\
        { return zBucketFile::seek(d); }\
    int update(type *d)\
        { return zBucketFile::update(d); }\
    const type *traverse(int n = 0)\
        { return (const type *)\
            zBucketFile::traverse(n); }\
    int error() { return zBucketFile::error(); }\
private:\
    zGBucketFile(type)(const zGBucketFile(type) &);\
    zGBucketFile(type) &operator=(const zGBucketFile(type) &);\
}
#endif
