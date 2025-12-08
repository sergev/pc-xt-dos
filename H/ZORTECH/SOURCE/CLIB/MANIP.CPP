#include <iomanip.hpp>

static ios &sfill(ios &s, int c)
{
    s.fill(c);
    return s;
}

SMANIP(int) setfill(int c)
{
    return SMANIP(int)(sfill, c);
}

static ios &swidth(ios &s, int w)
{
    s.width(w);
    return s;
}

SMANIP(int) setw(int w)
{
    return SMANIP(int)(swidth, w);
}

static ios &sprec(ios &s, int p)
{
    s.precision(p);
    return s;
}

SMANIP(int) setprecision(int p)
{
    return SMANIP(int)(sprec, p);
}

static ios &rsf(ios& s, long v)
{
    s.unsetf(v);
    return s;
}

SMANIP(long) resetiosflags(long v)
{
    return SMANIP(long)(rsf, v);
}

static ios &sf(ios& s, long v)
{
    s.flags(v|s.flags());
    return s;
}

SMANIP(long) setiosflags(long v)
{
    return SMANIP(long)(sf, v);
}

