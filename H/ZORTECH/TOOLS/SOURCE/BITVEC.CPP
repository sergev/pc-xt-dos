#include <stdio.h>
#include <bitvec.hpp>

// The first two zBitVec16 and zBitVec32 constructors are tiny, but
// we will generally adopt a policy of not inlining constructors.
// When the compiler has finished with them they are inevitably more
// complex than would be imagined.
zBitVec16::zBitVec16(unsigned INT16 v)
{
    body = v;
}

zBitVec32::zBitVec32(unsigned long v)
{
    body = v;
}

zBitVec16::zBitVec16(const zBitVec &a)
{
    memmove(&body,a.bp(),2);
}

zBitVec32::zBitVec32(const zBitVec &a)
{
    memset(&body,0,4);
    memmove(&body,a.bp(),a.capacity() >> 3);
}

zBitVec16 &zBitVec16::operator=(const zBitVec &a)
{
    memmove(&body,a.bp(),2);
    return *this;
}

zBitVec32 &zBitVec32::operator=(const zBitVec &a)
{
    memset(&body,0,4);
    int n = a.capacity();
    n = n > 32? 32: n;
    memmove(&body,a.bp(),n >> 3);
    return *this;
}

// The first zBitVecX constructor takes a size_t
// argument, and sets up zBitVecX object of size
// suitable to hold n bits.
zBitVecX::zBitVecX(size_t n)
{
    nbits = (n | 7)+1;
    body = new unsigned char[nbits/8];
    if (!body) { err = ENOMEM; return; }
    memset(body,'\0',nbits/8);
}

// The copy constructor for zBitVecX is much the same as those for
// the other variants, except that it has first got to allocate the
// memory to copy the vector into.
zBitVecX::zBitVecX(const zBitVec &a)
{
    nbits = a.capacity();
// Replicate the bitmap of the source object
    body = new unsigned char[nbits/8];
    if (!body) { err = ENOMEM; return; }
    memmove(body,a.bp(),nbits/8);
}

// Again the implementation of assignment is basically as it was
// for the fixed size variants except that this time we have to check
// first that an object is not being assigned to itself. Then the
// existing storage has to be freed, and a new block of the required
// size allocated.
zBitVecX &zBitVecX::operator=(const zBitVecX &a)
{
// Check if we're assigning an object to itself
    if ((void *) this == (void *) &a)
        return *this;
// Junk the existing bitmap
    delete body;
// Replicate the bitmap of the source object
    nbits = a.nbits;
    body = new unsigned char[nbits/8];
    if (!body) { err = ENOMEM; return *this; }
    memmove(body,a.body,nbits/8);
    return *this;
}

zBitVecX &zBitVecX::operator=(const zBitVec &a)
{
// Check if we're assigning an object to itself
    if ((void *) this == (void *) &a)
        return *this;
// Junk the existing bitmap
    delete body;
// Replicate the bitmap of the source object
    nbits = a.capacity();
    body = new unsigned char[nbits/8];
    if (!body) { err = ENOMEM; return *this; }
    memmove(body,a.bp(),nbits/8);
    return *this;
}

// The set and reset functions for zBitVecX have to check first
// that the argument is within acceptable bounds.  If so they calculate and
// set or reset the appropriate bit.
int zBitVecX::set(size_t i)
{
    if (i < 0 || i >= nbits)
        return BITVEC_OUTBOUNDS;
    body[i >> 3] |= 1 << (i & 7);
    return BITVEC_OK;
}

int zBitVecX::reset(size_t i)
{
    if (i < 0 || i >= nbits)
        return BITVEC_OUTBOUNDS;
    body[i >> 3] &= ~(1 << (i & 7));
    return BITVEC_OK;
}
