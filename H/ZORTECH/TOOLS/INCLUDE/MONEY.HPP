#ifndef MONEY_HPP
#define MONEY_HPP
#include <tools.hpp>

enum money_errors {
    MONEY_OK,
    MONEY_OVERFLOW,
    MONEY_DIV0
};

class zMoney {
public:

// A set of constructors provides for initialization of a zMoney
// object from a variety of sources.  Just an int, so it is
// possible to say zMoney x = 0, an explicit long/int pair, a
// double, and also a character string - as in zMoney
// y = "999999999.99". There is also an explicit copy constructor.
    zMoney();
    zMoney(long d, int c);
    zMoney(int);
    zMoney(double);
    zMoney(const char *);
    zMoney(const zMoney&);

// There is also a set of assignment operators matching most of the
// constructors which provide assignment from the built in types without
// the need to incidentally call constructors.
    zMoney &operator=(const zMoney &a)
        { _dollars = a._dollars; _cents = a._cents; return *this; }
    zMoney &operator=(int n)
        { _dollars = n; _cents = 0; return *this; }
    zMoney &operator=(double d)
        { fromdouble(d); return *this; }
    zMoney &operator=(const char *s)
        { xlate(s); return *this; }

// The unary operators minus and not are provided by member functions
// returning a zMoney object and an int respectively.
    zMoney operator-() const;
    int operator!() const
        { return !_dollars && !_cents; }

// Operations which affect the object to which they are applied are
// also implemented as members. Multiplication and division are by doubles.
    zMoney &operator+=(const zMoney&);
    zMoney &operator-=(const zMoney&);
    zMoney &operator*=(double);
    zMoney &operator/=(double);

// An automatic conversion to a double is provided, which makes it
// possible to use zMoney objects as arguments to math functions.
// It will also allow the use of a zMoney object in an if statement.
    operator double() const
        { return double(_dollars)+double(_cents)/100.0; }

// We also want to be able to multiply a money object by a factor to
// produce a new money object.  Note that multiplying a double by a
// zMoney will produce a double!
    zMoney operator*(double) const;

// For money it also seems appropriate to provide a percentage function.
    zMoney percent(double a) const
        { return *this * (a/100.0); }

// As in the cases of other new numeric types we have defined, a
// format function provides for representation of a zMoney object
// as a justified string.  The values of the dollars and cents are
// also available as separate entities if you need to format it
// some other way.
    char *format(char *s, int l = 0) const;
    long dollars() const { return _dollars; }
    int cents() const { return _cents; }

    int error() { int e = errf; errf = 0; return e; }

// Binary operations are provided by friend functions, so that the
// zMoney object involved can be on either side of the operator, with
// the other operand being any of the built in types for which
// an implicit conversion is provided.
    friend zMoney operator+(const zMoney&, const zMoney&);
    friend zMoney operator-(const zMoney&, const zMoney&);
    friend double operator/(const zMoney&, const zMoney&);
    friend zMoney operator%(const zMoney&, const zMoney&);

// Finally there are the customary selection of boolean operators,
// also implemented as friends.
    friend int operator==(const zMoney&, const zMoney&);
    friend int operator!=(const zMoney&, const zMoney&);
    friend int operator>(const zMoney&, const zMoney&);
    friend int operator<(const zMoney&, const zMoney&);
    friend int operator>=(const zMoney&, const zMoney&);
    friend int operator<=(const zMoney&, const zMoney&);
private:
    void carry();
    void fromdouble(double);
    void xlate(const char *);

    long _dollars;
    INT16 _cents;
    static INT16 errf;
};

// Some of the boolean functions are small enough to be inlined.
// They are included in the header file so that they can be expanded
// in any program module where they are required.
inline int operator==(const zMoney &a, const zMoney &b)
{
    return a._dollars == b._dollars && a._cents == b._cents;
}

inline int operator!=(const zMoney &a, const zMoney &b)
{
    return a._dollars != b._dollars || a._cents != b._cents;
}

inline int operator>=(const zMoney &a, const zMoney &b)
{
    return !(a < b);
}

inline int operator<=(const zMoney &a, const zMoney &b)
{
    return !(a > b);
}
#endif
