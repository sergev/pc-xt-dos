#ifndef __BITVEC_HPP
#define __BITVEC_HPP
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <tools.hpp>

enum BitVec_errors {
    BITVEC_OK,
    BITVEC_OUTBOUNDS = 20
};

class zBitVec {
public:
    virtual int operator[](size_t) const = 0;
    virtual int bit(size_t) const = 0;
    virtual int set(size_t) = 0;
    virtual int reset(size_t) = 0;
    virtual size_t capacity() const = 0;

    virtual ~zBitVec() {}
protected:
    virtual char *bp() const = 0;
    static int err;
};

class zBitVec16 : public zBitVec {
public:
    zBitVec16(unsigned INT16 = 0);
    zBitVec16(const zBitVec&);
    zBitVec16 &operator=(unsigned a)
        { body = a; return *this; }
    zBitVec16 &operator=(const zBitVec16 &a) { body = a.body; return *this; }
    zBitVec16 &operator=(const zBitVec&);

    int operator[](size_t i) const
        { return (body & (1 << (i & 0xf)))? 1: 0; }
    int bit(size_t i) const
        { return (body & (1 << (i & 0xf)))? 1: 0; }
    int set(size_t i)
        { body |= (1 << (i & 0xf)); return 0; }
    int reset(size_t i)
        { body &= ~(1 << (i & 0xf)); return 0; }
    size_t capacity() const { return 16; }
private:
    char *bp() const { return (char *) &body; }
    unsigned INT16 body;
};

class zBitVec32 : public zBitVec {
public:
    zBitVec32(unsigned long = 0);
    zBitVec32(const zBitVec&);
    zBitVec32 &operator=(unsigned long a)
        { body = a; return *this; }
    zBitVec32 &operator=(const zBitVec32 &a) { body = a.body; return *this; }
    zBitVec32 &operator=(const zBitVec&);

    int operator[](size_t i) const
        { return (body & (1L << (i & 0x1f)))? 1: 0; }
    int bit(size_t i) const
        { return (body & (1L << (i & 0x1f)))? 1: 0; }
    int set(size_t i)
        { body |= (1L << (i & 0x1f)); return 0; }
    int reset(size_t i)
        { body &= ~(1L << (i & 0x1f)); return 0; }
    size_t capacity() const { return 32; }
private:
    char *bp() const { return (char *) &body; }
    unsigned long body;
};

class zBitVecX : public zBitVec {

    friend class zSet;
public:
    zBitVecX(size_t);
    zBitVecX(const zBitVec&);
    zBitVecX &operator=(const zBitVecX&);
    zBitVecX &operator=(const zBitVec&);

    int operator[](size_t i) const
        { return (body[i >> 3] & (1 << (i & 7)))? 1: 0; }
    int bit(size_t i) const
        { return (body[i >> 3] & (1 << (i & 7)))? 1: 0; }
    int set(size_t);
    int reset(size_t);
    size_t capacity() const { return nbits; }

    ~zBitVecX() { delete body; }
private:
    char *bp() const { return (char *) body; }
    size_t nbits;
    unsigned char *body;
};
#endif
