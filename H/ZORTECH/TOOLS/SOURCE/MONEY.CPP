#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <money.hpp>

zMoney::zMoney()
{
    _dollars = 0;
    _cents = 0;
}

// The first constructor with arguments provides for explicit conversion
// of dollars and cents to zMoney.  It copes sensibly with arbitrary dollar
// and cent arguments zMoney(10,-101) via the carry function
zMoney::zMoney(long a, int b)
{
    _dollars = a;
    _dollars += b/100;
    _cents = b%100;
    carry();
    errf = 0;
}

// The second argument with a constructor is a conversion from a double
// to zMoney.  It checks for overflow by comparing the whole part
// of the double argument with LONG_MAX,  a constant defined as per
// ANSI C in limits.h. If the source is too big, zMoney is set to the
// maximum possible value, and the error flag set to signal overflow.
void zMoney::fromdouble(double cd)
{
    double w, f, d = cd;
    int negative = d < 0? (d = -d, 1): 0;
                        // Convert to positive
    f = modf(d,&w);     // Separate whole and fractional parts
    if (w > LONG_MAX) { // Check for overflow
        _dollars = LONG_MAX;
        _cents = 99;
        errf = MONEY_OVERFLOW;
    } else {
        int c = f*1000;
        _cents = c/10;
        if (c%10 >= 5)  // Round the cents
            ++_cents;
        _dollars = w;
        errf = 0;
    }
    if (negative) {     // Adjust signs
        _dollars = -_dollars; _cents = -_cents;
    }
}

zMoney::zMoney(int n)
{
    fromdouble(double(n));
}

zMoney::zMoney(double d)
{
    fromdouble(d);
}

// The conversion from a character string also checks for overflow,
// and sets the maximum value if it is detected.
void zMoney::xlate(const char *s)
{
    const char *p = s;
    int of = 0, sign = 0;
    if (*p == '-') {
        sign = 1;
        ++p;
    }
    _dollars = _cents = 0;
    for (;;) {
        if (!*p || !isdigit(*p)) break;
        if (LONG_MAX/10 < _dollars) {
            _dollars = LONG_MAX;
            of = MONEY_OVERFLOW;
            break;
        }
        _dollars *= 10;
        int n = *p++ - '0';
        if (LONG_MAX-n < _dollars) {
            _dollars = LONG_MAX;
            of = MONEY_OVERFLOW;
            break;
        }
        _dollars += n;
    }
    if (of)
        _cents = 99;
    else {
        if (*p == '.') {
            ++p;
            if (isdigit(*p)) {
                _cents = 10*(*p++ - '0');
                if (isdigit(*p))
                    _cents += *p - '0';
            }
        }
    }
    if (sign) {
        _dollars = -_dollars;
        _cents = -_cents;
    }
    errf = of;
}

zMoney::zMoney(const char *s)
{
    xlate(s);
}

zMoney::zMoney(const zMoney &a)
{
    _cents = a._cents;
    _dollars = a._dollars;
}

// Carry is a private function designed to correct the situation where
// the signs of dollars and cents do not match.  This will arise during
// several arithmetic operations.  It also deals with the situation
// when cents are for some reason overflowed.
void zMoney::carry()
{
    while (_cents < 0 && _dollars > 0) {
        --_dollars;
        _cents += 100;
    }
    while (_cents > 0 && _dollars < 0) {
        ++_dollars;
        _cents -= 100;
    }
    _dollars += _cents/100;
    _cents %= 100;
}

char *zMoney::format(char *s, int l) const
{
    char *p = s, buf[12];
    int sign = 0, lj = l < 0;
    if (lj) l = -l;
    sprintf(buf,"%ld", _dollars < 0? -_dollars: _dollars);
    int t = strlen(buf)+3;
    if (_dollars < 0 || _cents < 0) {
        sign = 1;
        ++t;
    }
    l -= t;
    if (!lj)
        for (;l > 0; --l) *p++ = ' ';
    if (sign)
        *p++ = '-';
    *p = '\0';
    sprintf(p,"%s.%02d", buf, _cents < 0? -_cents: _cents);
    p = s+strlen(s);
    for (;l > 0; --l) *p++ = ' ';
    *p = '\0';
    return s;
}

zMoney operator+(const zMoney &a, const zMoney &b)
{
    zMoney t;
    t._dollars = a._dollars+b._dollars;
    t._cents = a._cents+b._cents;
    t.carry();
    return t;
}

zMoney zMoney::operator-() const
{
    zMoney t = *this;
    t._dollars = -t._dollars;
    t._cents = -t._cents;
    return t;
}

zMoney operator-(const zMoney &a, const zMoney &b)
{
    zMoney t;
    t._dollars = a._dollars-b._dollars;
    t._cents = a._cents-b._cents;
    t.carry();
    return t;
}

zMoney &zMoney::operator+=(const zMoney &a)
{
    _dollars += a._dollars;
    _cents += a._cents;
    carry();
    return *this;
}

zMoney &zMoney::operator-=(const zMoney &a)
{
    _dollars -= a._dollars;
    _cents -= a._cents;
    carry();
    return *this;
}

zMoney zMoney::operator*(double a) const
{
    a *= (_dollars+_cents/100.0);
    return zMoney(a);
}

zMoney &zMoney::operator*=(double d)
{
    d *= (_dollars+_cents/100.0);
    fromdouble(d);
    return *this;
}

// Division should ideally yield a value of integral type - that is
// how may b's can be got out of a.  However there is no integral
// type which will hold the result of the worst case (dividing the
// maximum money by one cent),  so we have to use a double return
// value.
double operator/(const zMoney &a, const zMoney &b)
{
    if (!b) {
        zMoney::errf = MONEY_DIV0;
        return 0;
    }
    double n = a._dollars+a._cents/100.0;
    double d = b._dollars+b._cents/100.0;
    double q = n/d;
    modf(q,&n);
    if (n > LONG_MAX) {
        zMoney::errf = MONEY_OVERFLOW;
        n = LONG_MAX;
    }
    return n;
}

// Modulo also has to resort to floating point arithmetic.
zMoney operator%(const zMoney &a, const zMoney &b)
{
    if (!b) {
        zMoney::errf = 0;
        return zMoney(0,0);
    }
    int neg = a._dollars < 0;
    double n = a._dollars+a._cents/100.0;
    double d = b._dollars+b._cents/100.0;
    double q = n/d;
    q = modf(q,&n);
    if (n > LONG_MAX)
        zMoney::errf = MONEY_OVERFLOW;
    return zMoney(q);
}

zMoney &zMoney::operator/=(double d)
{
    d /= (_dollars+_cents/100.0)/d;
    fromdouble(d);
    return *this;
}

int operator>(const zMoney &a, const zMoney &b)
{
    long t = a._dollars - b._dollars;
    return t? t > 0: a._cents > b._cents;
}

int operator<(const zMoney &a, const zMoney &b)
{
    long t = a._dollars-b._dollars;
    return t ? t < 0: a._cents < b._cents;
}
