//_ Implementation code for class IntNN
//_ INTNN.CPP
//_
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <intnn.hpp>

// Format constructs a string representing the zIntNN in buffer b,
// then returns a pointer to it.  Length l governs padding, if
// it is zero there is none. If negative, the number is left justified
// otherwise it is right justified.
char *zIntNN::format(char *b, int l) const
{
    int i, n = 2*BYTES, rj = l > 0;
    const unsigned char *p = body+BYTES-1;
    char *q = b;

    l= l < 0? -l: l;
    if (sign && l) --l;
    for (; p > body && !*p; --p)
        n -= 2;
    if (!(*p >> 4))
        --n;        // count the high zeroes
    if (rj)
        for (; l > n; --l) *q++ = ' ';
    int m = n;
    if (sign)
        *q++ = '-';
    if (n & 1) {
        *q++ = (*p-- & 0xf) + '0';
        --n;
    }
    for (; n; n -= 2, --p) {
        *q++ = (*p >> 4) + '0';
        *q++ = (*p & 0xf) + '0';
    }
    for (; l > m; --l) *q++ = ' ';
    *q = '\0';
    return b;
}

// Test checks the instance to see if it is non-zero. If it is it
// returns a non-zero value reflecting the sign of the instance,
// otherwise zero.
int zIntNN::test() const
{
    if (memtest(body,BYTES))
        return (sign? -1: 1);
    return 0;
}

// The left shift operator is used to signify nibble shifting, i.e.
// Multiplication by ten.  Nibble manipulation is only required if the
// required number of shifts is odd.  Even then only one call to
// bcd_mul10 is required, since even shifts can be handled by just
// memmoving bytes.  Left shifting may cuse overflow, so the error
// function should be checked out after a left shift.
zIntNN zIntNN::operator<<(int n) const
{
    if (!n)
        return *this;

    int of = 0;

    zIntNN local = *this;
    if (n & 1) {                // if odd power * by 10
        of += bcd_mul10(BYTES,local.body);
        --n;
    }
    n /= 2;
    if (n) {
        unsigned char *p = local.body+BYTES-1;
        for (int i = n; i--;)
            if (*p--)
                ++of;           // check if anything in top n bytes
        memmove(local.body+n,local.body,BYTES-n);
        memset(local.body,'\0',n);
    }
    local.status = of? INT20_OVERFLOW: 0;
    return local;
}

zIntNN &zIntNN::operator<<=(int n)
{
    if (!n)
        return *this;

    int of = 0;

    if (n & 1) {                // if odd power * by 10
        of += bcd_mul10(BYTES,body);
        --n;
    }
    n /= 2;
    if (n) {
        unsigned char *p = body+BYTES-1;
        for (int i = n; i--;)
            if (*p--)
                ++of;           // check if anything in top n bytes
        memmove(body+n,body,BYTES-n);
        memset(body,'\0',n);
    }
    status = of? INT20_OVERFLOW: 0;
    return *this;
}

// Right shift does divides by ten and uses one call to bcd_div10 if
// an odd number of shifts are required.  Right shift is benign.  Its
// worst case is that the number is simply zeroed.
zIntNN zIntNN::operator>>(int n) const
{
    if (!n)
        return *this;

    zIntNN local = *this;
    if (n & 1) {        // if odd power divide by 10
        bcd_div10(BYTES,local.body);
        --n;
    }
    n /= 2;
    if (n) {
        memmove(local.body,local.body+n,BYTES-n);
        memset(local.body+BYTES-n,'\0',n);
    }
    return local;
}

zIntNN &zIntNN::operator>>=(int n)
{
    if (!n)
        return *this;

    if (n & 1) {        // if odd power divide by 10
        bcd_div10(BYTES,body);
        --n;
    }
    n /= 2;
    if (n) {
        memmove(body,body+n,BYTES-n);
        memset(body+BYTES-n,'\0',n);
    }
    return *this;
}

// Lmul is just plain long multiplication. It returns a non-zero value
// if overflow occurs.
int zIntNN::lmul(const zIntNN& s)
{
    unsigned char acc[80], buf[80],
        *b = (unsigned char *) s.body;
    short of = 0, i,j, digit;

    memset(acc,'\0',BYTES);
    for (i = BYTES; i--; ++b) {
        if (!memtest(b,i))
            break;                      // rest of multiplier zero
        digit = *b & 0xf;
        if (digit) {
            memmove(buf,body,BYTES);
            of += bcd_mul(BYTES,buf,digit); // mul by low nibble digit
            of += bcd_add(BYTES,acc,buf);   // add to accumulator
        }
        of += bcd_mul10(BYTES,body);        // * 10 for next digit
        digit = *b >> 4;
        if (digit) {
            memmove(buf,body,BYTES);
            of += bcd_mul(BYTES,buf,digit); // mul by high nibble
            of += bcd_add(BYTES,acc,buf);   // add to accumulator
        }
        if (i)                          // don't overflow on limit
            of += bcd_mul10(BYTES,body);    // * 10 for next digit
    }
    memmove(body,acc,BYTES);
    return of;
}

// Ldiv provides for both divide and modulo so it modifies its
// argument - beware!  The zIntNN for which it is called (*this)
// gets the quotient, while the argument zIntNN gets the remainder.
int zIntNN::ldiv(zIntNN& b)
{
    unsigned char *p, quot[80], test[80], hold[80];
    int n, m;

    for (m = BYTES*2, p = body+(BYTES-1); !*p && m; --p, m -= 2) ;
    if (!m) return 0;       // dividend 0 - nothing to do
    if (!(*p & 0xf0))
        --m;        // gets number of significant digits in dividend
    for (n = BYTES*2, p = b.body+(BYTES-1); !*p && n; --p, n -= 2) ;
    if (!n) {
        status = -1;        // dividing by zero
        return -1;
    }
    if (!(*p & 0xf0))
        --n;        // gets significant digits in divisor
    if (m < n) {
        memmove(b.body,body,BYTES);    // won't go, fill in remainder
        memset(body,'\0',BYTES);       // and zero quotient
        return 0;
    }
    memset(quot,'\0',BYTES); // ready quotient
    n = m-n;                 // work out how much to shift divisor
    b <<= n;                 // shift
    ++n;                     // set iteration count
    for (; n--;) {                      // iterate
        for (; memrcmp(body,b.body,BYTES) >= 0;) {
            bcd_inc(BYTES,quot);       // will subtract, adjust quotient
            bcd_sub(BYTES,body,b.body);
        }
        bcd_div10(BYTES,b.body);       // divisor/10
        if (n > 0)
            bcd_mul10(BYTES,quot);     // quotient*10 if any more
    }
    memmove(b.body,body,BYTES);        // report remainder
    memmove(body,quot,BYTES);          // and quotient
    return 0;
}

// Signed_add figures out whether what is needed is actually an
// addition or a subtraction. The sign of the b argument is taken
// not from b, but from the third argument, so the function can
// also be used for subtractions by reversing the sign indicator.
zIntNN zIntNN::signed_add(const zIntNN &a, const zIntNN &b, int bs)
{
    zIntNN loc;
    if (a.sign ^ bs) {  // signs differ 
        int mag = memrcmp(a.body,b.body,BYTES);
        if (mag > 0) {
            loc = a;
            bcd_sub(BYTES,loc.body,b.body);
        } else if (mag < 0) {
            loc = b;
            loc.sign = bs;
            bcd_sub(BYTES,loc.body,a.body);
        }
    } else {
        loc = a;
        int of = bcd_add(BYTES,loc.body,b.body);
        if (of) loc.status = INT20_OVERFLOW;
    }
    return loc;
}

// Setup is a helper function for the constructors from integral
// types.  It is normally passed a long,  which will have been
// explicitly cast from the initializing type.  However it has a
// defaulted second argument which allows it to deal with an
// unsigned long as well.  Setup is benign - none of the built in
// types can cause overflow.
void zIntNN::setup(long sn, int uns)
{
    sign = status = 0;
    memset(body,'\0',BYTES);
    if (!sn) return;
    unsigned long n;

    int digit;
    unsigned char *d = body;

    if (uns)
        n = (unsigned long) sn;
    else {
        if (sn < 0) {
            sign = 1;                   // fill in sign
            sn = -sn;
        }
        n = sn;
    }
    while (n) {
        digit = n % 10;
        n /= 10;
        *d = digit;                 // fill in low nibble
        if (!n)
            break;                  // nothing left to fill in
        digit = n % 10;
        n /= 10;
        *d += digit << 4;           // then high one
        ++d;
    }
    
}

// Xlate provides a similar service for character strings.  In this
// case overflow is a possiblity, so check the object state after
// initializing or assigning from a character string if the string
// value isn't known at compile time.
void zIntNN::xlate(const char *s)
{
    sign = status = 0;
    memset(body,'\0',BYTES);
    int i, t;
    const char *q;
    unsigned char *p = body;

    t = strlen(s);
    if (!t)
        return;
    if (*s == '-') {
        q = s+1;
        sign = 1;
    } else {
        q = s;
        sign = 0;
    }
    s += t-1;           // start with least significant
    for (i = BYTES*2; s >= q && i;) {
        *p = *s-'0';    // low nibble
        --s;
        --i;
        if (!(s >= q && i))
            break;      // no more string or no room
        t = (*s-'0') << 4;
        *p += t;        // add high nibble
        ++p;
        --s;
        --i;
    }
    if (s >= q)         // some string left report overflow
        status = INT20_OVERFLOW;
    if (!test())
        sign = 0;
}

// The constructor with no arguments provides a zero instance;
zIntNN::zIntNN() { memset(body,0,BYTES); sign = status = 0; }

// The integral promotions just cast their argument to long if
// required, then use setup();
zIntNN::zIntNN(char a) { setup(long(a)); }
zIntNN::zIntNN(unsigned char a) { setup(long(a)); }
zIntNN::zIntNN(int a) { setup(long(a)); }
zIntNN::zIntNN(unsigned a) { setup(long(a)); }
zIntNN::zIntNN(long a) { setup(a); }
zIntNN::zIntNN(unsigned long a) { setup(a,1); }

// The constructor from a string uses xlate().
zIntNN::zIntNN(const char *a) { xlate(a); }

zIntNN::zIntNN(double a)
{
    char buf[_ND+2]; // allow for sign and terminating null
    if (a < INTNN_MAXFLOAT) {
        sprintf(buf,"%-1.0f",a);
        xlate(buf);
    } else {
        memset(body,0x99,BYTES);
        sign = 0;
        status = INT20_OVERFLOW;
    }
}

// Assignment from another zIntNN is just straight memberwise assignment.
zIntNN &zIntNN::operator=(const zIntNN &a)
{
    memmove(body,a.body,BYTES);
    sign = a.sign; status = a.status;
    return *this;
}

// For double we use the constructor then copy the temporary variable
// into *this;
zIntNN &zIntNN::operator=(double a)
{
    zIntNN loc = a;
    *this = loc;
    return *this;
}

// Negation just makes a copy, changes its sign then returns the copy.
zIntNN zIntNN::operator-() const
{
    zIntNN loc = *this;
    loc.sign = !loc.sign;
    return loc;
}

// This is the first of the friend functions, inplementing binary +.
// One of the arguments will be an actual zIntNN,  otherwise this
// function would not have got called,  but the other can be anything that
// one of the constructors can handle, i.e. all the integral types,
// a double, or a pointer to a string.

// Notice also that addition may cause overflow, which will be noted
// in the returned object.  The test for zero on the local object is
// neccessary if zero objects with a negative sign are to be avoided.
zIntNN operator+(const zIntNN &a, const zIntNN &b)
{
    return a.signed_add(a,b,b.sign);
}

// The += operation is a member function, but similar to the friend
// above.
zIntNN &zIntNN::operator+=(const zIntNN &a)
{
    *this = signed_add(*this,a,a.sign);
    return *this;
}

zIntNN operator-(const zIntNN &a, const zIntNN &b)
{
    return a.signed_add(a,b,!b.sign);
}

zIntNN &zIntNN::operator-=(const zIntNN &a)
{
    *this = signed_add(*this,a,!a.sign);
    return *this;
}

// The ++ operator has to check the sign to determine if what is
// required is actually a bcd_inc() or a bcd_dec.
zIntNN &zIntNN::operator++()
{
    int of;
    if (sign)
        of = bcd_dec(BYTES,body);
    else
        of = bcd_inc(BYTES,body);
    if (!test())
        sign = 0;
    status = of? INT20_OVERFLOW: 0;
    return *this;
}

// Operator-- is similar except that some checking for zero crossing
// is neccessary to avoid negative zeroes.
zIntNN &zIntNN::operator--()
{
    int of = 0;
    if (!memtest(body,BYTES)) {
        bcd_inc(BYTES,body);
        sign = 1;
    } else if (sign)
        of = bcd_inc(BYTES,body);
    else
        of = bcd_dec(BYTES,body);
    status = of? INT20_OVERFLOW: 0;
    return *this;
}

zIntNN zIntNN::operator++(int)
{
    zIntNN t = *this;
    operator++();
    return t;
}

zIntNN zIntNN::operator--(int)
{
    zIntNN t = *this;
    operator--();
    return t;
}

// Multiplication and division have to work out the sign of the result
// then call lmul() or ldiv() to do the real work.
zIntNN operator*(const zIntNN &a, const zIntNN &b)
{
    zIntNN loc(a);

    loc.sign = loc.sign != b.sign? 1: 0;
    int of = loc.lmul(b);
    if (!loc.test())
        loc.sign = 0;
    loc.status = of? INT20_OVERFLOW: 0;
    return loc;
}

zIntNN &zIntNN::operator*=(const zIntNN &a)
{
    sign = (sign != a.sign)? 1: 0;
    int of = lmul(a);
    if (!test())
        sign = 0;
    status = of? INT20_OVERFLOW: 0;
    return *this;
}

// Division won't cause overflow, but might be by zero.
zIntNN operator/(const zIntNN &a, const zIntNN &b)
{
    if (!b) {
        zIntNN t;
        t.status = INT20_DIV0;
        return t;
    }
    zIntNN loc1(a), loc2(b);

    loc1.sign = a.sign != b.sign? 1: 0;
    int of = loc1.ldiv(loc2);
    if (of)
        loc1.status = INT20_DIV0;
    if (!loc1.test())
        loc1.sign = 0;
    return loc1;             // gets quotient
}

zIntNN &zIntNN::operator/=(const zIntNN &a)
{
    if (!a) {
        memset(body,0,BYTES);
        status = INT20_DIV0;
        return *this;
    }
    zIntNN loc = a;

    sign = (sign != a.sign)? 1: 0;
    int of = ldiv(loc);
    if (!test())
        sign = 0;
    status = of? INT20_OVERFLOW: 0;
    return *this;             // gets quotient
}

zIntNN operator%(const zIntNN &a, const zIntNN &b)
{
    if (!b) {
        zIntNN t;
        t.status = INT20_DIV0;
        return t;
    }
    zIntNN loc1(a), loc2(b);

    int of = loc1.ldiv(loc2);
    if (!loc2.test())
        loc2.sign = 0;
    else loc2.sign = a.sign;
    loc2.status = of? INT20_OVERFLOW: 0;
    return loc2;               // gets remainder
}

zIntNN &zIntNN::operator%=(const zIntNN &a)
{
    if (!a) {
        memset(body,0,BYTES);
        status = INT20_DIV0;
        return *this;
    }
    zIntNN loc = a;

    sign = (sign != a.sign)? 1: 0;
    int of = ldiv(loc);
    *this = loc;
    if (!test())
        sign = 0;
    status = of? INT20_OVERFLOW: 0;
    return *this;               // gets remainder
}

// The explicit conversion puts the long result in the variable
// refferred to by the reference argument.  If truncation occurs
// the function returns INT20_OVERFLOW.
int zIntNN::tolong(long &t) const
{
    long v = 0L;
    const unsigned char *p;

    if (*this > LONG_MAX || *this < LONG_MIN) {
// do the best we can!
        t = sign? LONG_MIN: LONG_MAX;
        return INT20_OVERFLOW;
    }

// skip over zeros
    for (p = body+BYTES-1; !*p; --p) ;
    for (; p >= body; --p) {
        v *= 10;
        v += *p >> 4;
        v *= 10;
        v += *p & 0xf;
    }
    if (sign)
        v = -v;
    t = v;
    return 0;
}

// This is similar to the stuff in the standard C library which is
// usen by scanf to convert strings to the ANSI floating point
// representation.
zIntNN::operator double() const
{
    const unsigned char *p;
    double dval = 0.0, exp = 1.0;
    unsigned long msdec = 0, lsdec = 0;
    unsigned long msscale = 1;

    p = body+BYTES-1;
    while (!*p)
        --p;
    while (p >= body)
    {
        if (msdec < (0x7FFFFFFF-10)/10)
            msdec = msdec*10 + (*p >> 4);
        else if (msscale < (0x7FFFFFFF-10)/10)
        {
            lsdec = lsdec*10 + (*p >> 4);
            msscale *= 10;
        }
        else
            exp *= 10;
        if (msdec < (0x7FFFFFFF-10)/10)
            msdec = msdec*10 + (*p & 0xf);
        else if (msscale < (0x7FFFFFFF-10)/10)
        {
            lsdec = lsdec*10 + (*p & 0xf);
            msscale *= 10;
        }
        else
            exp *= 10;
        --p;
    }
    dval = msdec;
    if (msscale != 1)        /* if stuff was accumulated in lsdec */
        dval = dval * msscale + lsdec;
    dval *= exp;
    return (sign) ? -dval : dval;
}

// The comparison operators have to consider the signs of the two
// objects.  If this does not resolve the comparison, then the bytes
// are compared starting at the most significant end - high memory.
int operator<(const zIntNN &a, const zIntNN &b)
{
    if (a.sign && !b.sign)
        return 1;
    if (!a.sign && b.sign)
        return 0;
    if (a.sign && b.sign)
        return memrcmp(a.body,b.body,BYTES) > 0;
    return memrcmp(a.body,b.body,BYTES) < 0;
}

int operator>(const zIntNN &a, const zIntNN &b)
{
    if (a.sign && !b.sign)
        return 0;
    if (!a.sign && b.sign)
        return 1;
    if (a.sign && b.sign)
        return memrcmp(a.body,b.body,BYTES) < 0;
    return memrcmp(a.body,b.body,BYTES) > 0;
}
