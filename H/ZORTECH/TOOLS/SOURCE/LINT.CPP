#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <bios.h>
#include <ctype.h>
#include <lint.hpp>

typedef unsigned long ulong;

int zLintN::umul(const zLintN &a)
{
// Set up a product buffer big enough to hold the largest possible result,
    unsigned INT16 prod[WORDS*2];
// and zero it.
    memset(prod,0,sizeof(prod));

// For each digit of the multiplier
    for (int j = 0; j < WORDS; ++j) {
        unsigned INT16 carry = 0;
        for (int i = 0; i < WORDS; ++i) {
            long t = ((ulong) a.body[j])*body[i] + prod[i+j] + carry;
            prod[i+j] = t&0xffff;
            carry = t >> 16;
        }
        prod[i+j] = carry;
    }
    int of = memtest(&prod[WORDS],BYTES) || (prod[WORDS-1] & 0x8000);
    memmove(body,prod,BYTES);
    return of;
}

void pr(unsigned INT16 *r, int n)
{
    for (; n > 0; --n)
        printf("%04x ",r[n-1]);
    puts("\n");
}

void zLintN::hex()
{
    for (int n = 4; n > 0; --n)
        printf("%04x ",body[n-1]);
    puts("\n");
}

int zLintN::udiv(zLintN &a)
{
    if (!memtest(a.body,BYTES))    // division by zero
        return LINT_DIV0;
    if (!memtest(body,BYTES)) {    // no quotient, no remainder
        memset(a.body,0,BYTES);
        return 0;
    }
    if (!memcmp(body,a.body,BYTES)) { // once exactly
        memset(body,0,BYTES);
        *body = 1;
        memset(a.body,0,BYTES);
        return 0;
    }
    int ldv, ld;
    for (unsigned INT16 *p = a.body+(WORDS-1); !*p; --p) ;
    if (p == a.body) {               // single digit division
        unsigned INT16 rem = lint_div(body,a.body[0],WORDS);
        memset(a.body,0,BYTES);
        a.body[0] = rem;
        return 0;
    } else  // get no of digits in divisor and dividend
        ld = p-a.body+1;
    for (p = body+(WORDS-1); !*p; --p) ;
    ldv = p-body+1;
    if (ldv <= ld) {     // may not go
        if (a.body[ld-1] > body[ld-1]) {    // no it wont
            memmove(a.body,body,BYTES);     // move dividend to remainder
            memset(body,0,BYTES);
            return 0;
        }
    }
#ifdef DEBUG
printf("ld = %d, ldv = %d\n",ld,ldv);
#endif

// Now we need to get serious. First reserve some space for normalized
// versions of dividend and divisor
    unsigned INT16 dvnd[WORDS+1], divr[WORDS+1];

// and for the quotient, and a register to do the multiplications in
    unsigned INT16 reg[WORDS+1], quot[WORDS];
    dvnd[WORDS] = 0; divr[WORDS] = 0;
    memmove(dvnd,body,BYTES); memmove(divr,a.body,BYTES);
    memset(quot,0,BYTES);

    unsigned INT16 msd = divr[ld-1], nsd;
    unsigned INT16 normalizer = 0x10000L/(msd+1);

#ifdef DEBUG
pr(dvnd,WORDS+1);
pr(divr,WORDS+1);
printf("normalizer = %04x\n",normalizer);
#endif
    if (normalizer > 1) {
        lint_mul(dvnd,normalizer,WORDS+1);
        lint_mul(divr,normalizer,WORDS+1);
    }

// work out how many digits to process in quotient - qd thro 0
    int qd = ldv-ld;
                           
    msd = divr[ld-1], nsd = divr[ld-2];
                            // reset most and next significant divisor digits
    for (; qd >= 0; --qd) {
        unsigned INT16 qt;
        ulong t = 0;
        if (dvnd[ldv] == msd) qt = 0xffff;
        else {
            t = *((ulong *) &dvnd[ldv-1]);
            qt = t/msd;
#ifdef DEBUG
printf("ld = %d, ldv = %d\n",ld,ldv);
printf("t = %08lx, msd = %04x, qt = %04x\n",t,msd,qt);
#endif
            t %= msd;
            if (((ulong) qt) * nsd >
                    (t << 16) + dvnd[ldv-2])
                --qt;
#ifdef DEBUG            
puts("decremented");
#endif
        }
#ifdef DEBUG
printf("qd = %d, qt = %04x\n",qd,qt);
pr(dvnd,WORDS+1);
pr(divr,WORDS+1);
#endif
        memmove(reg,divr,BYTES+2);
        lint_mul(reg,qt,ld+1);
        if (memrcmp(reg,&dvnd[qd],2*ld+2) > 0) { // qt was one too big
            lint_sub(reg,divr,ld+1);
#ifdef DEBUG
puts("adjusted");
#endif
            --qt;
        }
#ifdef DEBUG
pr(reg,WORDS+1);
#endif
        lint_sub(&dvnd[qd],reg,ld+1);
        quot[qd] = qt;
        --ldv;
#ifdef DEBUG
pr(dvnd,WORDS+1);
bioskey(0);
#endif
    }
    lint_div(dvnd,normalizer,WORDS);
    memmove(a.body,dvnd,BYTES);
    memmove(body,quot,BYTES);
    return 0;
}

char *zLintN::format(char *s, int l) const
{
    char buf[80];
    unsigned INT16 t[WORDS];
    memmove(t,body,BYTES);
    int sign = 0;
    if (isneg()) {
        sign = 1;
        lint_negate(t,WORDS);
    }
    unsigned INT16 tt[WORDS+1];
    for (int i = WORDS; i > 0; --i)
        tt[i] = lint_div(t,10000,WORDS);
    tt[0] = *t;
    char *q = buf;
    if (!memtest(tt,(WORDS+1)*2))
        sprintf(buf,"0");
    else  {
        for (i = 0; i < WORDS+1; ++i) {
            if (!tt[i]) continue;
            sprintf(q,"%u",tt[i++]);
            break;
        }
        q = buf+strlen(buf);
        for (; i < WORDS+1; ++i, q += 4)
            sprintf(q,"%04u",tt[i]);
    }
    int rl = strlen(buf);
    if (sign)
        ++rl;
    q = s;
    if (l < 0) {
        l = -l;
        for (; l > rl; --l) *q++ = ' ';
    }
    if (sign) *q++ = '-';
    sprintf(q,"%s",buf);
    q = s+strlen(s);
    for (; l > rl; --l) *q++ = ' ';
    *q = '\0';
    return s;
}

int zLintN::ldiv(zLintN &d, int divide)
{
    int signr, signn = 0, signd = 0;
    if (isneg()) {
        lint_negate(body,WORDS);
        signn = 1;
    }
    if (d.isneg()) {
        lint_negate(d.body,WORDS);
        signd = 1;
    }
    signr = signn ^ signd;
    int err = udiv(d);
    if (err) return err;
    if (divide)
        if (signr) lint_negate(body,WORDS);
    else 
        if (signn) lint_negate(d.body,WORDS);
    return 0;
}

int zLintN::lmul(const zLintN &b)
{
    int signp, signa = 0, signb = 0;
    if (isneg()) {
        lint_negate(body,WORDS);
        signa = 1;
    }
    zLintN loc = b;
    if (loc.isneg()) {
        lint_negate(loc.body,WORDS);
        signb = 1;
    }
    signp = signa ^ signb;
    int err = umul(loc);
    if (signp) lint_negate(body,WORDS);
    return err;
}

void zLintN::setup(long v, int uns)
{
    if (uns || v >= 0)
        memset(body,0,BYTES);
    else
        memset(body,0xff,BYTES);
    memmove(body,&v,sizeof(long));
    status = 0;
}

void zLintN::xlate(const char *s)
{
    memset(body,'\0',BYTES);
    int base = 10, sign, i, t;
    const char *q;
    t = strlen(s);
    if (!t)
        return;
    if (*s == '-') {
        --t;
        q = s+1;
        sign = 1;
    } else {
        q = s;
        sign = 0;
    }
    if (*q == '0') {
        ++q;
        --t;
        if (*q == 'x' || *q == 'X') {
            base = 16;
            --t;
            ++q;
        } else 
            base = 8;
    }
    int chunks = t/4;
    int rest = t%4;
    if (rest)
        ++chunks;
    else
        rest = 4;
    unsigned INT16 chs, dummy[WORDS];
    INT16 of = 0;
    memset(dummy,0,BYTES);
    for (i = chunks; i--;) {
        if (base == 16) {
            of = body[3] != 0;
            memmove(&body[1],&body[0],6);
            body[0] = 0;
        } else {
            chs = (base == 10)? 10000: 010000;
            of = lint_mul(body,chs,WORDS);
        }
        if (of) break;
        for (dummy[0] = 0; rest--;) {
            switch (base) {
            case 0:
                if (*q > '7') {
                    status = LINT_BADCONST;
                    return;
                }
            case 1:
                if (!isdigit(*q)) {
                    status = LINT_BADCONST;
                    return;
                }
                break;
            case 2:
                if (!isxdigit(*q)) {
                    status = LINT_BADCONST;
                    return;
                }
            }
            if (base == 16)
                dummy[0] = dummy[0]*base +
                        ((*q > '9')? tolower(*q)-'a'+10: *q-'0');
            else
                dummy[0] = dummy[0]*base + *q - '0';
            ++q;
        }
        rest = 4;
        of = lint_add(body,dummy,WORDS);
        if (of) break;
    }
    if (of) status = LINT_OVERFLOW;
    if (sign) lint_negate(body,WORDS);
}

zLintN::operator double() const
{
    int sign;
    zLintN t = *this;
    if ((sign = isneg()) != 0)
        lint_negate(t.body,WORDS);
    double f = 0x10000L, v = 0.0;
    for (int i = WORDS-1; i >= 0; --i)
        v = v*f + body[i];
    if (sign)
        v = -v;
    return v;
}

int zLintN::tolong(long &dest) const
{
    if (isneg()) {
        zLintN t = LONG_MIN;
        if (*this < t) { dest = LONG_MIN; return -1; }
    } else {
        zLintN t = LONG_MAX;
        if (*this > t) { dest = LONG_MAX; return -1; }
    }
    dest = *((long *) body);
    return 0;
}

// The constructor with no arguments provides a zero instance;
zLintN::zLintN() { memset(body,0,BYTES); }

// The integral promotions just cast their argument to long if
// required, then use setup();
zLintN::zLintN(char a) { setup(long(a)); }
zLintN::zLintN(unsigned char a) { setup(long(a)); }
zLintN::zLintN(int a) { setup(long(a)); }
zLintN::zLintN(unsigned a) { setup(long(a)); }
zLintN::zLintN(long a) { setup(a); }
zLintN::zLintN(unsigned long a) { setup(a,1); }

// The constructor from a string uses xlate().
zLintN::zLintN(const char *a) { xlate(a); }

// Your program has probably already loaded the printf code to
// decompose doubles so we may as well reuse it.
zLintN::zLintN(double a)
{
    char buf[21];
    if (a < LINTN_MAXFLOAT) {
        sprintf(buf,"%-1.0f",a);
        xlate(buf);
    } else {
        memset(body,0xff,BYTES);
        body[WORDS-1] ^= 0x8000;
        if (a < 0.0)
            lint_negate(body,WORDS);
        status = LINT_OVERFLOW;
    }
}

// Assignment from another zLintN is just straight bitwise copy.
zLintN &zLintN::operator=(const zLintN &a)
{
    memmove(body,a.body,BYTES);
    return *this;
}

// For double we use the constructor then copy the temporary variable
// into *this.
zLintN &zLintN::operator=(double a)
{
    zLintN loc = a;
    *this = loc;
    return *this;
}

zLintN zLintN::operator-() const
{
    zLintN loc = *this;
    lint_negate(loc.body,WORDS);
    return loc;
}

zLintN operator+(const zLintN &a, const zLintN &b)
{
    zLintN t = a;
    t.status = lint_add(t.body,b.body,WORDS);
    return t;
}

zLintN &zLintN::operator+=(const zLintN &a)
{
    status = lint_add(body,a.body,WORDS);
    return *this;
}

zLintN operator-(const zLintN &a, const zLintN &b)
{
    zLintN t = a;
    t.status = lint_sub(t.body,b.body,WORDS);
    return t;
}

zLintN &zLintN::operator-=(const zLintN &a)
{
    status = lint_sub(body,a.body,WORDS);
    return *this;
}

zLintN operator*(const zLintN &a, const zLintN &b)
{
    zLintN t = a;
    t.status = t.lmul(b);
    return t;
}

zLintN operator/(const zLintN &a, const zLintN &b)
{
    zLintN t1 = a, t2 = b;
    t1.status = t1.ldiv(t2,1);
    return t1;
}

zLintN operator%(const zLintN &a, const zLintN &b)
{
    zLintN t1 = a, t2 = b;
    t2.status = t1.ldiv(t2,0);
    return t2;
}

zLintN &zLintN::operator*=(const zLintN &a)
{
    status = lmul(a);
    return *this;
}

zLintN &zLintN::operator/=(const zLintN &a)
{
    zLintN t = a;
    status = ldiv(t,1);
    return *this;
}

zLintN &zLintN::operator%=(const zLintN &a)
{
    zLintN t = a;
    status = ldiv(t,0);
    memmove(body,t.body,BYTES);
    return *this;
}

zLintN zLintN::operator&(const zLintN &a) const
{
    zLintN t = *this;
    for (int i = 0; i < WORDS; ++i)
        t.body[i] &= a.body[i];
    return t;
}

zLintN zLintN::operator|(const zLintN &a) const
{
    zLintN t = *this;
    for (int i = 0; i < WORDS; ++i)
        t.body[i] |= a.body[i];
    return t;
}

zLintN zLintN::operator^(const zLintN &a) const
{
    zLintN t = *this;
    for (int i = 0; i < WORDS; ++i)
        t.body[i] ^= a.body[i];
    return t;
}

zLintN &zLintN::operator&=(const zLintN &a)
{
    for (int i = 0; i < WORDS; ++i)
        body[i] &= a.body[i];
    return *this;
}

zLintN &zLintN::operator|=(const zLintN &a)
{
    for (int i = 0; i < WORDS; ++i)
        body[i] |= a.body[i];
    return *this;
}

zLintN &zLintN::operator^=(const zLintN &a)
{
    for (int i = 0; i < WORDS; ++i)
        body[i] ^= a.body[i];
    return *this;
}

zLintN zLintN::operator~() const
{
    zLintN t = *this;
    for (int i = 0; i < WORDS; ++i)
        t.body[i] = ~t.body[i];
    return t;
}

zLintN &zLintN::operator<<=(int n)
{
    if (n < 0) return this->operator>>(n);
    int words = n / 16;
    n %= 16;
    if (words) {
        if (words < WORDS) {
            memmove(body+words,body,(WORDS-words)*2);
            memset(body,0,words*2);
        } else {
            memset(body,0,BYTES);
            return *this;
        }
    }
    lint_lshift(body,n,WORDS);
    status = 0;
    return *this;
}

zLintN &zLintN::operator>>=(int n)
{
    if (n < 0) return this->operator<<(n);
    int words = n / 16;
    n %= 16;
    int fill = isneg()? 0xff: 0;
    if (words) {
        if (words < WORDS) {
            memmove(body,body+words,(WORDS-words)*2);
            memset(body+(WORDS-words),fill,(WORDS-words)*2);
        } else {
            memset(body,0,BYTES);
            return *this;
        }
    }
    lint_rshift(body,n,WORDS);
    status = 0;
    return *this;
}

zLintN zLintN::operator<<(int n) const
{
    zLintN t = *this;
    return t <<= n;
}

zLintN zLintN::operator>>(int n) const
{
    zLintN t = *this;
    return t >>= n;
}

zLintN zLintN::operator++(int)
{
    zLintN t = *this;
    status = lint_inc(body,WORDS);
    return t;
}

zLintN zLintN::operator--(int)
{
    zLintN t = *this;
    status = lint_dec(body,WORDS);
    return t;
}

int operator>(const zLintN &a, const zLintN &b)
{
    zLintN t = a-b;
    return !(t.body[WORDS-1] & 0x8000) && memtest(t.body,BYTES);
}

int operator<(const zLintN &a, const zLintN &b)
{
    zLintN t = b-a;
    return !(t.body[WORDS-1] & 0x8000) && memtest(t.body,BYTES);
}
