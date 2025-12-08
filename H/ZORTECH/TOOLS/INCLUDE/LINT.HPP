#ifndef __LINT_HPP
#define __LINT_HPP
#include <string.h>
#include <tools.hpp>

// modify these if bigger integers required
// LINTN_MAXFLOAT is pow(2, _NW*16)
#define zLintN zLint4
#define _NW 4
#define LINTN_MAXFLOAT 1.844674407370948e+19

extern "C" {
    int lint_add(unsigned INT16 *, const unsigned INT16 *, size_t);
    int lint_sub(unsigned INT16 *, const unsigned INT16 *, size_t);
    void lint_negate(unsigned INT16 *, size_t);
    int lint_inc(unsigned INT16 *, size_t);
    int lint_dec(unsigned INT16 *, size_t);
    int lint_mul(unsigned INT16 *, unsigned INT16, size_t);
    int lint_div(unsigned INT16 *, unsigned INT16, size_t);
    int lint_lshift(unsigned INT16 *, unsigned INT16, size_t);
    int lint_rshift(unsigned INT16 *, unsigned INT16, size_t);
    int memrcmp(const void*,const void*, size_t);
    int memtest(const void*, size_t);
}

enum lint_errors {
    LINT_OK,
    LINT_OVERFLOW,
    LINT_DIV0,
    LINT_BADCONST
};

const int WORDS = _NW;
const int BYTES = WORDS*2;

class zLintN {

public:

    zLintN();

// The first set of constructors provide for integral promotions, so
// that an zLintN can be initialized from any of the other integral types.
    zLintN(char a);
    zLintN(unsigned char a);
    zLintN(int a);
    zLintN(unsigned a);
    zLintN(long a);
    zLintN(unsigned long a);

// We also need to be able to perform implicit conversions from floating
// point to zLintN, so a constructor with a double argument is required
// too.  After that there is a departure from what can be done with
// the built in integral types, since it is also possible to initialize
// an zLintN from a character string.  Thes are the nearest we can get
// to an zLintN constant. In this case the string will be interpreted
// as octal if it starts with 0, or as hexadecimal if it starts with
// 0x or 0X.
    zLintN(double);
    zLintN(const char *a);

// A set of assignment operators match the constructors.  These return
// references for the usual reason, that is for efficiency, and so that
// multiple assignments can be "stacked" on the same statement line.
    zLintN &operator=(const zLintN &a);
    zLintN &operator=(const char a)
        { setup(long(a)); return *this; }
    zLintN &operator=(unsigned char a)
        { setup(long(a)); return*this; }
    zLintN &operator=(int a)
        { setup(long(a)); return *this; }
    zLintN &operator=(unsigned a)
        { setup(long(a)); return *this; }
    zLintN &operator=(long a)
        { setup(a); return *this; }
    zLintN &operator=(unsigned long a)
        { long t = long(a); setup(t,1); return *this; }

    zLintN &operator=(double a);
    zLintN &operator=(const char *a) { xlate(a); return *this; }

// The operators which affect the object for which they are called are
// provided by member functions.  When appropriate they take zLintN
// reference arguments, which means they can be matched using the
// implicit conversions defined above for the integral types,
// doubles, and character strings.
    zLintN &operator+=(const zLintN&);
    zLintN &operator-=(const zLintN&);
    zLintN &operator*=(const zLintN&);
    zLintN &operator/=(const zLintN&);
    zLintN &operator%=(const zLintN&);

    zLintN &operator++()
        { status = lint_inc(body,WORDS); return *this; }
    zLintN &operator--()
        { status = lint_dec(body,WORDS); return *this; }
    zLintN operator++(int);
    zLintN operator--(int);

// For the twos complement integer types we need the full set of bit
// and shift operations.
    zLintN operator&(const zLintN&) const;
    zLintN operator|(const zLintN&) const;
    zLintN operator^(const zLintN&) const;
    zLintN &operator&=(const zLintN&);
    zLintN &operator|=(const zLintN&);
    zLintN &operator^=(const zLintN&);
    zLintN operator<<(int) const;
    zLintN operator>>(int) const;
    zLintN &operator<<=(int);
    zLintN &operator>>=(int);
    zLintN operator~() const;

// A unary minus operator, and the not operator are provided.
    zLintN operator-() const;
    int operator!() const { return !memtest(body,WORDS); }

// Some capability to convert to the built in types is also required.
// Conversion to double is reasonably safe,  as it will not give rise
// to a loss of information.  However conversion to the built in
// integral types is quite likely to result in truncation, so only
// an explicit conversion, returning an error value is provided.
    operator double() const;
    int tolong(long&) const;

// A utility function provides in general for output.  This can be
// used if required to define an overload of ostream::operator<<().
    char *format(char *, int = 0) const;
    void hex();     // A useful debugging function!

// The final member function in the public interface allows for
// interrogation of an object status.  The error flag is reset
// by the query.
    int error()
        { int e = status; status = 0; return e; }

// Binary operations are provided by friend functions taking zLintN
// reference arguments so that they can take advantage of the defined
// implicit conversions.
    friend zLintN operator+(const zLintN&, const zLintN&);
    friend zLintN operator-(const zLintN&, const zLintN&);
    friend zLintN operator*(const zLintN&, const zLintN&);
    friend zLintN operator/(const zLintN&, const zLintN&);
    friend zLintN operator%(const zLintN&, const zLintN&);

// The full set of comparison functions must also be provided.
    friend int operator==(const zLintN&, const zLintN&);
    friend int operator!=(const zLintN&, const zLintN&);
    friend int operator>(const zLintN&, const zLintN&);
    friend int operator<(const zLintN&, const zLintN&);
    friend int operator>=(const zLintN&, const zLintN&);
    friend int operator<=(const zLintN&, const zLintN&);

private:
    int isneg() const { return body[WORDS-1] & 0x8000; }
    int umul(const zLintN&);
    int lmul(const zLintN&);
    int udiv(zLintN&);
    int ldiv(zLintN&, int);
    void setup(long, int = 0);
    void xlate(const char *);

    unsigned INT16 body[WORDS];
    static INT16 status;
};

//. Some of the friend functions are just a function call and can
//. therefore be inlined.
inline int operator==(const zLintN &a, const zLintN &b)
    { return !memcmp(a.body, b.body, BYTES); }
inline int operator!=(const zLintN &a, const zLintN &b)
    { return memcmp(a.body, b.body, BYTES); }
inline int operator>=(const zLintN &a, const zLintN &b)
    { return !(b > a); }
inline int operator<=(const zLintN &a, const zLintN &b)
    { return !(a > b); }
#endif
