#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sets.hpp>

// Default constructor
zSet::zSet() : bv(S_MAX) {}

// Constructor from a string, for example:-
// "3, 5, 7, 11 - 15, 19, 21 - 23"
// Out of range values are ignored
zSet::zSet(const char *s) : bv(S_MAX)
{
    if (!*s) return;
    for (const char *p = s;;) {
        int i, j, nr;
        for (; isdigit(*p); ++p) ;
        for (; isspace(*p); ++p) ;
        nr = sscanf((char *) s,"%d",&i);
        if (!nr) return;
        if (*p == '-') {
            for (++p; isspace(*p); ++p) ;
            s = p;
            nr = sscanf((char *) s,"%d",&j);
            if (!nr) return;
            i = (i < 0)? 0: i;
            i = (i > S_MAX)? S_MAX: i;
            j = (j < 0)? 0: j;
            j = (j > S_MAX)? S_MAX: j;
            if (i > j) {
                int t = i;
                j = i;
                i = t;
            }
            for (; i <= j; bv.set(i), ++i) ;
            for (; isdigit(*s); ++s) ;
            for (; isspace(*s); ++s) ;
            if (*s != ',') return;
            for (s++; isspace(*s); ++s) ;
            p = s;
        } else if (!*p || *p == ',') {
            if (i >=0 && i <= S_MAX)
                bv.set(i);
            if (!*p) return;
            for (++p; isspace(*p); ++p) ;
            s = p;
        } else return;
    }
}

// Constructor which fixes up the set from a list of
// n integers.  Out of range values are ignored.
zSet::zSet(int n, int *list) : bv(S_MAX)
{
    for (int i = 0; i < n; ++i) {
        if (list[i] >=0 && list[i] <= S_MAX)
            bv.set(list[i]);
    }
}

zSet::zSet(const zSet &a) : bv(S_MAX)
{
    bv = a.bv;
}

int zSet::operator!() const
{
    unsigned char *p = bv.body;
    for (int i = 0; i < S_MAX/8; ++i, ++p)
        if (*p) return 0;
    return 1;
}

// Set union is the bitwise or of the respective bitmaps.
zSet operator+(const zSet &a, const zSet &b)
{
    zSet t;
    
    int *tp = t.body();
    const int *p = a.body(), *q = b.body();
    for (int i = 0; i < S_MAX/16; ++i) {
        *tp = *p | *q;
        ++tp; ++p; ++q;
    }
    return t;
}

// Set difference is exclusive or.
zSet operator-(const zSet &a, const zSet &b)
{
    zSet t;
    int *tp = t.body();
    int *p = a.body(), *q = b.body();
    for (int i = 0; i < S_MAX/16; ++i) {
        *tp = *p ^ *q;
        ++tp; ++p; ++q;
    }
    return t;
}

// Set intersection is bitwise and.
zSet operator*(const zSet &a, const zSet &b)
{
    zSet t;
    int *tp = t.body();
    const int *p = a.body(), *q = b.body();
    for (int i = 0; i < S_MAX/16; ++i) {
        *tp = *p & *q;
        ++tp; ++p; ++q;
    }
    return t;
}

// The subset qualification must be established by
// examining each candidate in turn until an exception
// is found, or all have been checked.
int operator<=(const zSet &a, const zSet &b)
{
    for (int i = 0; i < S_MAX; ++i)
        if (b.bv[i] && !a.bv[i]) return 0;
    return 1;
}
