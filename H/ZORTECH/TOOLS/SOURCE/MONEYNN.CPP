#include <stdio.h>
#include <bios.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <moneynn.hpp>

void zMoneyNN::fromdouble(double d)
{
    int neg = 0;
    double w, f = d;
    if (f < 0) {
        neg = 1;
        f = -f;
    }
    f *= 100.0;
    f = modf(f,&w);
    if (f >= 0.5)
        w += 1.0;
    _cents = neg? -w: w;
}

void zMoneyNN::xlate(const char *s)
{
    char buf[24];
    strncpy(buf,s,23);
// copy one too many if overlength (21+sign+1)
    char *p = strchr(buf,'.');
    if (p) {
        *p++ = *(p+1);
        *p++ = *(p+1);
        *p = '\0';
    } else strcat(buf,"00");
    _cents = (const char *) buf;
// if its too long this will overflow the cents
}

zMoneyNN::zMoneyNN() : _cents(0) {}

zMoneyNN::zMoneyNN(int n)
{
    fromdouble(double(n));
}

zMoneyNN::zMoneyNN(long d, int c)
{
    double v = d+c/100.0;
    fromdouble(v);
}

zMoneyNN::zMoneyNN(double d)
{
    fromdouble(d);
}

zMoneyNN::zMoneyNN(const char *s)
{
    xlate(s);
}

zMoneyNN::zMoneyNN(const zMoneyNN &a)
{
    _cents = a._cents;
}

zMoneyNN &zMoneyNN::operator=(const zMoneyNN &a)
{
    memmove(this,&a,sizeof(zMoneyNN));
    return *this;
}

char *zMoneyNN::format(char *s, int l) const
{
    char buf[22], *p = buf, *q = p, *r = s;
    _cents.format(buf);
    if (*p == '-') ++p;
    int tl, n = strlen(p);
    if (n <= 2)
        tl = 4;
    else
        tl = n+1;
    if (*q == '-') ++tl;
    
    int lj = l < 0;
    if (lj) l = -l;

    l -= tl;
    if (!lj)
        for (;l > 0; --l) *r++ = ' ';
    if (*q == '-')
        *r++ = '-';
    if (n <= 2) {
        *r++ = '0';
        *r++ = '.';
        if (n == 1)
            *r++ = '0';
    } else {
        for (; n > 2; --n) *r++ = *p++;
        *r++ = '.';
    }
    while (n--) *r++ = *p++;
    for (;l > 0; --l) *r++ = ' ';
    *r = '\0';
    return s;
}

zMoneyNN zMoneyNN::operator-() const
{
    zMoneyNN t = *this;
    t = -t;
    return t;
}

zMoneyNN operator+(const zMoneyNN &a, const zMoneyNN &b)
{
    zMoneyNN t;
    t._cents = a._cents+b._cents;
    return t;
}

zMoneyNN operator-(const zMoneyNN &a, const zMoneyNN &b)
{
    zMoneyNN t;
    t._cents = a._cents-b._cents;
    return t;
}

zMoneyNN &zMoneyNN::operator+=(const zMoneyNN &a)
{
    _cents += a._cents;
    return *this;
}

zMoneyNN &zMoneyNN::operator-=(const zMoneyNN &a)
{
    _cents -= a._cents;
    return *this;
}

zMoneyNN zMoneyNN::operator*(double a) const
{
    a *= double(_cents);
    return zMoneyNN(a);
}

zMoneyNN &zMoneyNN::operator*=(double a)
{
    double whole;
    double frac = modf(a,&whole);
    if (!frac && (whole < LONG_MAX) && (whole > LONG_MIN)) {
        long t = whole;
        _cents *= t;
    } else {
        a *= double(_cents);
        _cents = a;
    }
    return *this;
}

zMoneyNN &zMoneyNN::operator/=(double a)
{
    double whole;
    double frac = modf(a,&whole);
    if (!frac && (whole < LONG_MAX) && (whole > LONG_MIN)) {
        long t = whole;
        _cents /= t;
    } else
        _cents /= a;
    return *this;
}

zIntNN operator/(const zMoneyNN &a, const zMoneyNN &b)
{
    return a._cents/b._cents;
}

zMoneyNN operator%(const zMoneyNN &a, const zMoneyNN &b)
{
    zMoneyNN t = a;
    t._cents %= b._cents;
    return t;
}

zIntNN zMoneyNN::dollars() const
{
    return _cents >> 2;
}

int zMoneyNN::cents() const
{
    char *p = (char *) &_cents;
    int t = 10*((*p & 0xf0) >> 4) + (*p % 0xf);
    return t;
}
