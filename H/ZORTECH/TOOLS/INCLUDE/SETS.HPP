#ifndef __SETS_HPP
#define __SETS_HPP
#include <string.h>
#include <tools.hpp>
#include <bitvec.hpp>

// Redefine this if you want bigger sets - should be a multiple
// of 8 of course
#define S_MAX 128

class zSet {
public:

// No copy constructor or assignment operator are specified, since
// memberwise initialization and assignment should be adequate. Class
// zBitVecX makes provision for this to be done properly.
    zSet();

// The first initializing constructor allows for initialization
// by a comma separated string of integers and integer ranges.  The
// second provides for initialization from a list (array) of n integers.
    zSet(const char *);
    zSet(int n, int *list);
    zSet(const zSet&);

// Set inclusion
    int includes(int i) { return bv[i]; }

// Test for empty set
    int operator!() const;

// Inserting and removing a member
   zSet &operator+=(int i)
        { bv.set(i); return *this; }
   zSet &operator-=(int i)
        { bv.reset(i); return *this; }
  
// Union
    friend zSet operator+(const zSet&, const zSet&);
// Difference
    friend zSet operator-(const zSet&, const zSet&);
// Intersection
    friend zSet operator*(const zSet&, const zSet&);
// Identity and the converse
    friend int operator==(const zSet&, const zSet&);
    friend int operator!=(const zSet&, const zSet&);

// Subset tests
    friend int operator<=(const zSet&, const zSet&);
    friend int operator>=(const zSet&, const zSet&);
private:
    int *body() const { return (int *) bv.body; }

// zSet is just implemented via a zBitVecX
    zBitVecX bv;
};

inline int operator==(const zSet &a, const zSet &b)
{
    return !memcmp(&a.bv,&b.bv,S_MAX/8);
}

inline int operator!=(const zSet &a, const zSet &b)
{
    return memcmp(&a.bv,&b.bv,S_MAX/8);
}

inline int operator>=(const zSet &a, const zSet &b)
{
    return  b <= a;
}
#endif
