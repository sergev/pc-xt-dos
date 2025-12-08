#include <box.hpp>

zBox::zBox(INT16 r, INT16 c, INT16 h, INT16 w,
            const char *atl, INT16 sv)
: zRectangle(r,c,h,w),
zScreenArea(r,c,h,w,atl,sv)
{
    box(atl[0],0,0,h,w);
    ++tlr; ++tlc; --brr; --brc;
    maxr -= 2; maxc -= 2;
}

zBox::zBox(zScreenSpec &s, INT16 sv) :
zRectangle(s.row,s.col,s.high,s.wide),
zScreenArea(s.row,s.col,s.high,s.wide,s.atl,sv)
{
    box(s.atl[0],0,0,s.high,s.wide);
    ++tlr; ++tlc; --brr; --brc;
    maxr -= 2; maxc -= 2;
}

void zBox::title(const char *s)
{
    --tlr;
    int l = strlen(s);
    int w = brc-tlc-3;
    l = l > w? w: l;
    rowcol(0,w > l? (w-l)/2: 0);
    putch('[');
    w -=2;
    for (int i = 0; l--; ++i)
        putch(s[i]);
    putch(']');
    ++tlr;
    rowcol(0,0);
}

zBox::~zBox()
{
    --tlr; --tlc; ++brr; ++brc;
    maxr += 2; maxc += 2;
}
