#ifndef __MONEYNN_HPP
#define __MONEYNN_HPP
#include <stdio.h>
#include <tools.hpp>
#include <intnn.hpp>

#define zMoneyNN zMoney18

enum money_errors {
    MONEY_OK,
    MONEY_OVERFLOW,
    MONEY_DIV0
};

class zMoneyNN {
public:

// The same set of constructors/ implicit type conversions are
// provided as were provided for the zMoney class.
    zMoneyNN();
    zMoneyNN(long, int);
    zMoneyNN(int);
    zMoneyNN(double);
    zMoneyNN(const char *);
    zMoneyNN(const zMoneyNN&);

//. Also the same set of explicit assignment operators.  Notice that
//. neither initialization nor assignment from ÝzMoney have been
//. encouraged.  Mixing the two types seems somewhat perverse!
    zMoneyNN &operator=(const zMoneyNN&);
    zMoneyNN &operator=(int n)
        { fromdouble(double(n)); return *this; }
    zMoneyNN &operator=(double d)
        { fromdouble(d); return *this; }
    zMoneyNN &operator=(const char *s)
        { xlate(s); return *this; }

// In fact little has been done to the class definition except to
// change the name of the data type.  There is an exception, since
// given the underlying BCD type ,division can be handled more
// comprehensively.
    zMoneyNN operator-() const;
    int operator!() const { return !_cents; }

// The operations which affect the object for which they were called
// are implemented as member functions:
    zMoneyNN &operator+=(const zMoneyNN&);
    zMoneyNN &operator-=(const zMoneyNN&);
    zMoneyNN &operator*=(double);
    zMoneyNN &operator/=(double);

    operator double() const { return double(_cents)/100.0; }

// Provision is made for multiplying a money object by a double to
// produce a new money object, and for a similar percent function.
    zMoneyNN operator*(double) const;
    zMoneyNN percent(double a) const { return operator*(a/100); }

    char *format(char *, int = 0) const;
    zIntNN dollars() const;
    int cents() const;
    int error() { return _cents.error(); }

    friend zMoneyNN operator+(const zMoneyNN&, const zMoneyNN&);
    friend zMoneyNN operator-(const zMoneyNN&, const zMoneyNN&);

// Division in this case has the same meaning in principle as it
// did with the previous money type.  How many A's do I get for B,
// and how much will I have left.  The difference here is that our
// data representation will not overflow if we divide the maximum
// sum of money by one cent!  The result can be returned as a zIntNN.
    friend zIntNN operator/(const zMoneyNN&, const zMoneyNN&);
    friend zMoneyNN operator%(const zMoneyNN&, const zMoneyNN&);

    friend int operator==(const zMoneyNN&, const zMoneyNN&);
    friend int operator!=(const zMoneyNN&, const zMoneyNN&);
    friend int operator>(const zMoneyNN&, const zMoneyNN&);
    friend int operator<(const zMoneyNN&, const zMoneyNN&);
    friend int operator>=(const zMoneyNN&, const zMoneyNN&);
    friend int operator<=(const zMoneyNN&, const zMoneyNN&);
private:
    zIntNN _cents;
    void xlate(const char *);
    void fromdouble(double);
};

inline int operator==(const zMoneyNN &a, const zMoneyNN &b)
    { return a._cents == b._cents; }
inline int operator!=(const zMoneyNN &a, const zMoneyNN &b)
    { return a._cents != b._cents; }
inline int operator>(const zMoneyNN &a, const zMoneyNN &b)
    { return a._cents > b._cents; }
inline int operator<(const zMoneyNN &a, const zMoneyNN &b)
    { return a._cents < b._cents; }
inline int operator>=(const zMoneyNN &a, const zMoneyNN &b)
    { return !(a < b); }
inline int operator<=(const zMoneyNN &a, const zMoneyNN &b)
    { return !(a > b); }
#endif
