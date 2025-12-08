#ifndef __INT20_HPP
#define __INT20_HPP
#include <string.h>
#include <tools.hpp>

// Modify these #defines if more digits required
#define zIntNN zInt20
#define _ND 20
#define INTNN_MAXFLOAT 1e+20

enum IntNN_errors {
    INT20_OK,
    INT20_OVERFLOW,
    INT20_DIV0
};

const int BYTES = _ND/2;

// The following are assembler functions in BCDLOW.ASM
// they have C linkage, and they are responsible for nibble
// manipulations using the BCD operations in the machines
// instruction set.
extern "C" {
int bcd_add(size_t, unsigned char *, const unsigned char *);
int bcd_sub(size_t, unsigned char *, const unsigned char *);
int bcd_inc(size_t, unsigned char *);
int bcd_dec(size_t, unsigned char *);
int bcd_mul(size_t, unsigned char *, int);
int bcd_mul10(size_t, unsigned char *);
int bcd_div10(size_t, unsigned char *);
int memrcmp(const void*, const void*, size_t);
int memtest(const void*, size_t);
}

class zIntNN {

public:
    zIntNN();

// The first set of constructors provide for integral promotions, so
// ®IzIntNN;constructors¯ that an zIntNN can be initialized from any
// of the other integral types.
    zIntNN(char a);
    zIntNN(unsigned char a);
    zIntNN(int a);
    zIntNN(unsigned a);
    zIntNN(long a);
    zIntNN(unsigned long a);

// We also need to be able to perform implicit conversions from floating
// point to zIntNN, so a constructor with a double argument is required
// too.  After that there is a departure from what can be done with
// the built in integral types, since it is also possible to initialize
// an zIntNN from a character string.  Thes are the nearest we can get
// to an zIntNN constant.
    zIntNN(double);
    zIntNN(const char *a);

// A set of assignment operators match the constructors.  These return
// references for the usual reasons, it's efficient, and it allows
// multiple assignments to be "stacked" on the same statement line.
    zIntNN &operator=(const zIntNN &a);
    zIntNN &operator=(char a)
        { setup(long(a)); return *this; }
    zIntNN &operator=(unsigned char a)
        { setup(long(a)); return*this; }
    zIntNN &operator=(int a)
        { setup(long(a)); return *this; }
    zIntNN &operator=(unsigned a)
        { setup(long(a)); return *this; }
    zIntNN &operator=(long a)
        { setup(a); return *this; }
    zIntNN &operator=(unsigned long a)
        { setup(a,1); return *this; }

    zIntNN &operator=(double a);
    zIntNN &operator=(const char *a)
        { xlate(a); return *this; }

// The operators which affect the object for which they are called are
// provided by member functions.  When appropriate they take const zIntNN
// reference arguments, which means they can be matched using the
// implicit conversions defined above for the integral types, doubles,
// and character strings.
    zIntNN &operator+=(const zIntNN&);
    zIntNN &operator-=(const zIntNN&);
    zIntNN &operator*=(const zIntNN&);
    zIntNN &operator/=(const zIntNN&);
    zIntNN &operator%=(const zIntNN&);

    zIntNN &operator++();
    zIntNN &operator--();
    zIntNN operator++(int);
    zIntNN operator--(int);

// These next functions are a departure from the usage with the built in
// integral types, with which they provide for bit shifting.  Here they
// provide the corresponding nibble shifting,  multiplying or
// dividing by powers of ten rather than of two.
    zIntNN operator<<(int) const;
    zIntNN operator>>(int) const;
    zIntNN &operator<<=(int);
    zIntNN &operator>>=(int);

// A unary minus operator, and the not operator are provided.
    zIntNN operator-() const;
    int operator!() const { return !test(); }

// Some capability to convert to the built in types is also required.
// Conversion to double is reasonably safe,  as it will not give rise
// to a loss of information.  However conversion to the built in
// integral types is quite likely to result in truncation, so only
// one explicit conversion, returning an error value is provided for
// explicit conversion to a long.
    operator double() const;
    int tolong(long&) const;

// A utility function provides in general for output.  This can be
// used if required to define an overload of ostream::operator<<();
    char *format(char *, int = 0) const;

// The final member function in the public interface allows for
// interrogation of an object status.  The error flag is reset
// by the query.
    int error()
        { int e = status; status = 0; return e; }

// Binary operations are provided by friend functions taking zIntNN
// reference arguments so that they can take advantage of the defined
// implicit conversions.
    friend zIntNN operator+(const zIntNN&, const zIntNN&);
    friend zIntNN operator-(const zIntNN&, const zIntNN&);
    friend zIntNN operator*(const zIntNN&, const zIntNN&);
    friend zIntNN operator/(const zIntNN&, const zIntNN&);
    friend zIntNN operator%(const zIntNN&, const zIntNN&);

    friend int operator==(const zIntNN&, const zIntNN&);
    friend int operator!=(const zIntNN&, const zIntNN&);
    friend int operator>(const zIntNN&, const zIntNN&);
    friend int operator<(const zIntNN&, const zIntNN&);
    friend int operator>=(const zIntNN&, const zIntNN&);
    friend int operator<=(const zIntNN&, const zIntNN&);

private:
    void setup(long, int = 0);
    void xlate(const char *);
    int test() const;
    static zIntNN signed_add(const zIntNN&, const zIntNN&, int);
    int lmul(const zIntNN&);
    int ldiv(zIntNN&);

    unsigned char body[BYTES];
    char sign;
    char status;
};

// Some of the friend functions can be inlined
inline int operator==(const zIntNN &a, const zIntNN &b)
    { return !memcmp(a.body, b.body, BYTES); }
inline int operator!=(const zIntNN &a, const zIntNN &b)
    { return memcmp(a.body, b.body, BYTES); }
inline int operator>=(const zIntNN &a, const zIntNN &b)
    { return !(b > a); }
inline int operator<=(const zIntNN &a, const zIntNN &b)
    { return !(a > b); }
#endif
