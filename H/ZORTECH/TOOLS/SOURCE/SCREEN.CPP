//_ Copyright (C) 1989, 1990 Zortech Ltd
//_ Written by Steve Teale
//_ All Rights Reserved
//_ Implementation code - class zScreenArea
//_ SCREEN.CPP
//_
#include <screen.hpp>

#ifndef __OS2__

// backspace implements a destructive backspace in the most obvious
// way - by sending backspace, space, backspace.
#ifdef Cscreen1
void zScreenArea::backspace()
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    putch('\b',0);
    putch(' ');
    putch('\b',0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

#endif

//  cleareol and cleareop blank from the current coordinate to the
// end of the line and end of the screen area respectively.  Blanking
// is done in the current attribute.
#ifdef Cscreen2
void zScreenArea::cleareol()
{
    unsigned v = cat*256+' ';
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    for (int i = tlc+cc; i <= brc;
        disp_pokew(tlr+cr,i++,v)) ;
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

void zScreenArea::cleareop()
{
    unsigned v = cat*256+' ';
    int r = cr, c = cc;
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    for (; r <= maxr;) {
        disp_pokew(r+tlr,c+tlc,v);
        r = c < maxc? (++c, r): (c = 0, r+1);
    }
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// cursoff removes the cursor from the screen by parking it elsewhere.
// The class variable coff remembers we have done this,  because some
// off the disp functions have the unwanted effect of restoring it!
#ifdef Cscreen3
void zScreenArea::cursoff()
{
    disp_move(25,0);    // off the screen
    disp_flush();
    coff = 1;
}

// The cursor function without arguments just updates the actual
// cursor position to reflect the current zScreenArea coordinates.
// If the cursor was off it will restore it.
void zScreenArea::cursor()
{
    disp_move(cr+tlr, cc+tlc);
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_flush();
#ifndef NOZEQ
    zEQ.showmouse();
#endif
    coff = 0;
}

// The second cursor function takes row/column arguments,  and is
// equivalent to rowcol(r,c); cursor();
void zScreenArea::cursor(INT16 r, INT16 c)
{
    disp_move(tlr+r, tlc+c);
    cr = r; cc = c;
    coff = 0;
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_flush();
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// rowcol sets new values for the current coordinates for the
// zScreenArea. Whatever happens next will happen at these
// coordinates.  The cursor IS NOT affected.
void zScreenArea::rowcol(INT16 r, INT16 c)
{
    disp_move(tlr+r, tlc+c);
    cr = r; cc = c;
}

#endif

// putch behaves pretty well the same as putc does with stdout.
// We can't use the function name putc,  even as a class member,
// because it is implemented as a macro in STDIO.H.
#ifdef Cscreen4
void zScreenArea::putch(unsigned char c, INT16 one)
{
#ifndef NOZEQ
    if (one)
        zEQ.hidemouse();
#endif
    if (c >= ' ') {
        disp_setattr(cat);
        disp_move(tlr+cr,tlc+cc);
        disp_putc(c);
        if (cc == maxc) {
            cc = 0;
            goto newline;
        } else
            ++cc;
    } else {
        switch (c) {
        case '\n':
            cc = 0;
newline:
            if (cr == maxr) {
                disp_scroll(1,tlr,tlc,brr,brc,cat);
                if (coff) cursoff();
            } else
                ++cr;
            break;
        case '\b':
            if (cc)
                --cc;
            break;
        case '\t':
            cc |= 7;
            ++cc;
            if (cc > maxc) {
                cc = 0;
                goto newline;
            }
            break;
        }
        disp_move(tlr+cr,tlc+cc);
    }
#ifndef NOZEQ
    if (one)
        zEQ.showmouse();
#endif
}

void zScreenArea::puts(const char *s)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    while (*s) putch(*s++, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// printf just does pretty much what you would expect. It is implemented
// using the ANSI standard macros va_list and va_start. Because the
// actual output is done by zScreenArea::puts, the output is bounded
// by the zScreenArea.
#ifdef Cscreen5
int zScreenArea::printf(char *format,...)
{
    char buf[256];
    va_list arg_ptr;
    va_start(arg_ptr,format);
    int rv = vsprintf(buf,format,arg_ptr);
    puts(buf);
    return rv;
}
#endif

// scroll has been kept simple.  It either scrolls the area one line up
// (the default - no arguments) or down (non-zero argument).
#ifdef Cscreen6
void zScreenArea::scroll(INT16 direction)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_scroll(direction? -1: 1, tlr, tlc, brr, brc, cat);
    if (coff)
        cursoff();  // disp_scroll manages to restore the cursor.
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// update changes the character and attribute at the current coordinates.
// It is particularly useful if you want to modify the bottom right corner
// of the zScreenArea.  Using putch there would cause scrolling!
void zScreenArea::update(INT16 r, INT16 c, INT16 attchr)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_pokew(tlr+r,tlc+c,attchr);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// Functions setattr, hilite, and normal control
// colours/attributes. setattr changes the current attribute,  and
// makes the new value 'normal'.  hilight changes the attribute
// temporarily to a predetermined highlight attribute which was set
// by the constructor. normal puts it back to the current attribute.
#ifdef Cscreen7
void zScreenArea::setattr(INT16 a)
{
    cat = a;
    normatt = cat;      // DLB
    disp_setattr(a);
}

void zScreenArea::sethigh(INT16 a)
{
    highatt = a;        // DLB
}


void zScreenArea::hilite()
{
    cat = highatt;
    disp_setattr(cat);
}

void zScreenArea::normal()
{
    cat = normatt;
    disp_setattr(cat);
}
#endif

// unpack takes as its argument a pointer to an array of integers,
// which represent a screen display,  and unpacks it into its own
// area.  The display must be of a suitable size for the zScreenArea
// otherwise it will be garbled.  The stream of integers should consist
// of a leading integer giving the number of integers following it,
// followed by either single integers representing a character/attribute
// combination,  or groups of three integers introduced by the unique
// value -1 (0xffff) with the other two being a repetition count and
// a character/attribute.
#ifdef Cscreen8
void zScreenArea::unpack(const INT16 *p)
{
    int tc, row = 0, col = 0, burst;
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    for (int i = *p++; i--;) {
        if (*p == -1) {    // It's a repeat burst
            ++p;
            burst = *p++;
            tc = *p++;
            i -= 2;
        } else {
            tc = *p++;
            burst = 1;
        }
        for (; burst--;) {
            disp_pokew(row+tlr,col+tlc,tc);
            row = col < maxc?
                (++col, row): (col = 0, ++row);
        }
    }
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// chat changes n screen area positions from their existing
// attribute to attribute a.  It is useful for making menus
// and such things where it is neccessary to change the highlighting
// of a bar of text.
#ifdef Cscreen9
void zScreenArea::chat(INT16 n, INT16 a)
{
    int row = cr, col = cc;
    unsigned t;
    a *= 256;

#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    for (; n--;) {
        t = disp_peekw(row+tlr,col+tlc);
        disp_pokew(row+tlr,col+tlc,(t & 0xff)+a);
        row = col < maxc?
            (++col, row): (col = 0, ++row);
    }
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// box draws a box with top left corner at r, c which is
// h rows high and w rows wide.  There is no clipping.  It is
// up to the programmer to ensure that it will fit in the zScreenArea.
// The frame types available are as per the disp package.
void zScreenArea::box(INT16 frame, INT16 r, INT16 c, INT16 h, INT16 w)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_box(frame,cat,tlr+r,tlc+c,tlr+r+h-1,tlc+c+w-1);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

#ifdef Cscreen10

// Do nothing constructor for virtual base
zRectangle::zRectangle() {}

// The actual constructor
zRectangle::zRectangle(INT16 r, INT16 c, INT16 h, INT16 w)
{
    tlr = r; tlc = c;
    brr = tlr+h-1; brc = tlc+w-1;
    maxr = h-1;     maxc = w-1;
}

// The constructor has defaulted arguments which give a full 25x80 screen
// area.  The arguments r, c give the position of the top left corner,
// h, w give the height and width, a the current attribute,
// ha the highlight attribute.  Argument su indicates whether or not
// to save the screen under the area - non-zero to save.  If the underlying
// area is saved the new zScreenArea is automatically cleared.
zScreenArea::zScreenArea(INT16 r, INT16 c, INT16 h,
            INT16 w, const char *al, INT16 su)
: zRectangle(r,c,h,w)
{
    init(al,su);
}

// The constructor which takes the zScreenSpec reference argument does
// almost exactly the same.
zScreenArea::zScreenArea(const zScreenSpec &a, INT16 su) 
: zRectangle(a.row,a.col,a.high,a.wide)
{
    init(a.atl, su);
}

void zScreenArea::init(const char *al, INT16 su)
{
    cat = normatt = al[1];
    highatt = al[2];
    saveunder = su;
    cr = cc = 0;
    if (!disp_inited)
        disp_open();
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    disp_setattr(cat);
    if (su) {
#ifdef DOS386
        unsigned INT16 r = _getcp();
        oldrow = r/256;
        oldcol = r & 0xff;
#else
        union REGS r;
        r.x.ax = 0x0300;
        r.x.bx = 0;
        int86(0x10,&r,&r);
        oldrow = r.h.dh;    // Save old cursor position
        oldcol = r.h.dl;
#endif
        sb = new unsigned INT16[wide()*high()];
        if (!sb) {
            err = ENOMEM;
            saveunder = 0;
            return;
        }
        disp_peekbox((unsigned short *)sb,tlr,tlc,brr,brc);
        cleareop();
    } else sb = 0;
#ifndef NOZEQ
    zEQ.showmouse();
#endif
    err = 0;
}

// Finally it is the destructors job to restore what was under the
// zScreenArea area before it was created if we specified that it
// was to be saved.
zScreenArea::~zScreenArea()
{
    if (saveunder) {
#ifndef NOZEQ
        zEQ.hidemouse();
#endif
        disp_pokebox((unsigned short *)sb,tlr,tlc,brr,brc);
        disp_move(oldrow,oldcol);
        disp_flush();
        delete sb;
#ifndef NOZEQ
        zEQ.showmouse();
#endif
    }
}
#endif

#else
// OS2 version
// backspace implements a destructive backspace in the most obvious
// way - by sending backspace, space, backspace.
#ifdef Cscreen1
void zScreenArea::backspace()
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    putch('\b',0);
    putch(' ');
    putch('\b',0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

//  cleareol and cleareop blank from the current coordinate to the
// end of the line and end of the screen area respectively.  Blanking
// is done in the current attribute.
#ifdef Cscreen2
void zScreenArea::cleareol()
{
    cbuf[0] = ' ';
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    VioWrtNCell((PBYTE) cbuf, wide()-cc, tlr+cr, tlc+cc, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

void zScreenArea::cleareop()
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    cleareol();
    int h = brr-(tlr+cr);
    if (h > 0)
        VioScrollUp(tlr+cr+1, tlc, brr, brc, 
                    h, (PBYTE) cbuf, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// cursoff removes the cursor from the screen by parking it elsewhere.
// The class variable coff remembers we have done this.
#ifdef Cscreen3
void zScreenArea::cursoff()
{
    VIOCURSORINFO ci;
    VioGetCurType((PVIOCURSORINFO) &ci,0);
    ci.attr = 0xffff;
    VioSetCurType((PVIOCURSORINFO) &ci,0);
    coff = 1;
}

// The cursor function without arguments just updates the actual
// cursor position to reflect the current zScreenArea coordinates.
// If the cursor was off it will restore it.
void zScreenArea::cursor()
{
    VIOCURSORINFO ci;
    if (coff) {
        VioGetCurType((PVIOCURSORINFO) &ci,0);
        coff = ci.attr = 0;
        VioSetCurType((PVIOCURSORINFO) &ci,0);
    }
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    VioSetCurPos(tlr+cr, tlc+cc, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// The second cursor function takes row/column arguments,  and is
// equivalent to rowcol(r,c); cursor();
void zScreenArea::cursor(INT16 r, INT16 c)
{
    VIOCURSORINFO ci;
    if (coff) {
        VioGetCurType((PVIOCURSORINFO) &ci,0);
        coff = ci.attr = 0;
        VioSetCurType((PVIOCURSORINFO) &ci,0);
    }
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    cr = r; cc = c;
    VioSetCurPos(tlr+cr, tlc+cc, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// rowcol sets new values for the current coordinates for the
// zScreenArea. Whatever happens next will happen at these
// coordinates. The cursor is not effected.
void zScreenArea::rowcol(INT16 r, INT16 c)
{
    cr = r; cc = c;
}
#endif

// putch behaves pretty well the same as putc does with stdout.
// We can't use the function name putc,  even as a class member,
// because it is implemented as a macro in STDIO.H.
#ifdef Cscreen4
void zScreenArea::putch(unsigned char c, INT16 one)
{
#ifndef NOZEQ
    if (one)
        zEQ.hidemouse();
#endif
    if (c >= ' ') {
        cbuf[0] = c;
        VioWrtNCell((PBYTE) cbuf, 1, tlr+cr, tlc+cc, 0);
        if (cc == maxc) {
            cc = 0;
            goto newline;
        } else
            ++cc;
    } else {
        switch (c) {
        case '\n':
            cc = 0;
newline:
            if (cr == maxr) {
                cbuf[0] = ' ';
                VioScrollUp(tlr,tlc,brr,brc,1,(PBYTE) cbuf,0);
            } else
                ++cr;
            break;
        case '\b':
            if (cc)
                --cc;
            break;
        case '\t':
            cc |= 7;
            ++cc;
            if (cc > maxc) {
                cc = 0;
                goto newline;
            }
            break;
        }
    }
#ifndef NOZEQ
    if (one)
        zEQ.showmouse();
#endif
}

void zScreenArea::puts(const char *s)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    while (*s) putch(*s++, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// printf just does pretty much what you would expect. It is implemented
// using the ANSI standard macros va_list and va_start. Because the
// actual output is done by zScreenArea::puts, the output is bounded
// by the zScreenArea.
#ifdef Cscreen5
int zScreenArea::printf(char *format,...)
{
    char buf[256];
    va_list arg_ptr;
    va_start(arg_ptr,format);
    int rv = vsprintf(buf,format,arg_ptr);
    puts(buf);
    return rv;
}
#endif

// scroll has been kept simple.  It either scrolls the area one line up
// (the default - no arguments) or down (non-zero argument).
#ifdef Cscreen6
void zScreenArea::scroll(INT16 direction)
{
    cbuf[0] = ' ';
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    if (direction)
        VioScrollUp(tlr,tlc,brr,brc,1,(PBYTE) cbuf,0);
    else 
        VioScrollDn(tlr,tlc,brr,brc,1,(PBYTE) cbuf,0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// update changes the character and attribute at the current coordinates.
// It is particularly useful if you want to modify the bottom right corner
// of the zScreenArea.  Using putch there would cause scrolling!
void zScreenArea::update(INT16 r, INT16 c, INT16 attchr)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    VioWrtNCell((PBYTE) &attchr, 1, tlr+r, tlc+c, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// Functions setattr, hilite, and normal control
// colours/attributes. setattr changes the current attribute,  and
// makes the new value 'normal'.  hilight changes the attribute
// temporarily to a predetermined highlight attribute which was set
// by the constructor. normal puts it back to the current attribute.
#ifdef Cscreen7
void zScreenArea::setattr(INT16 a)
{
    cbuf[1] = a;
}

void zScreenArea::hilite()
{
    cat = highatt;
    cbuf[1] = cat;
}

void zScreenArea::normal()
{
    cat = normatt;
    cbuf[1] = cat;
}
#endif

// unpack takes as its argument a pointer to an array of integers,
// which represent a screen display,  and unpacks it into its own
// area.  The display must be of a suitable size for the zScreenArea
// otherwise it will be garbled.  The stream of integers should consist
// of a leading integer giving the number of integers following it,
// followed by either single integers representing a character/attribute
// combination,  or groups of three integers introduced by the unique
// value -1 (0xffff) with the other two being a repetition count and
// a character/attribute.
#ifdef Cscreen8
void zScreenArea::unpack(const INT16 *p)
{
    int tc, row = 0, col = 0, burst;
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    for (int i = *p++; i--;) {
        if (*p == -1) {    // It's a repeat burst
            ++p;
            burst = *p++;
            tc = *p++;
            i -= 2;
        } else {
            tc = *p++;
            burst = 1;
        }
        for (; burst--;) {
            VioWrtNCell((PBYTE) &tc, 1, row+tlr, col+tlc, 0);
            row = col < maxc?
                (++col, row): (col = 0, ++row);
        }
    }
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

// chat changes n screen area positions from their existing
// attribute to attribute a.  It is useful for making menus
// and such things where it is neccessary to change the highlighting
// of a bar of text.
#ifdef Cscreen9
void zScreenArea::chat(INT16 n, INT16 a)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    VioWrtNAttr((PBYTE) &a, n, tlr+cr, tlc+cc, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}

// box draws a box with top left corner at r, c which is
// h rows high and w rows wide.  There is no clipping.  It is
// up to the programmer to ensure that it will fit in the zScreenArea.
// The frame types available are as per the disp package.

static boxchars[2][6] =
    {{ '\xda', '\xc4', '\xbf', '\xb3', '\xc0', '\xd9' },
     { '\xc9', '\xcd', '\xbb', '\xba', '\xc8', '\xbc' }};

void zScreenArea::box(INT16 frame, INT16 r, INT16 c, INT16 h, INT16 w)
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    frame = frame? 1: 0;
    cbuf[0] = boxchars[frame][0];
    VioWrtNCell((PBYTE) cbuf,1, tlr+r, tlc+c, 0);
    cbuf[0] = boxchars[frame][1];
    VioWrtNCell((PBYTE) cbuf, w-2, tlr+r, tlc+c+1, 0);
    cbuf[0] = boxchars[frame][2];
    VioWrtNCell((PBYTE) cbuf, 1, tlr+r, tlc+c+w-1, 0);
    cbuf[0] = boxchars[frame][3];
    int lim = tlr+r+h-1;
    for (int i = tlr+r+1; i < lim; ++i) {
        VioWrtNCell((PBYTE) cbuf, 1, i, tlc+c, 0);
        VioWrtNCell((PBYTE) cbuf, 1, i, tlc+c+w-1, 0);
    }
    cbuf[0] = boxchars[frame][4];
    VioWrtNCell((PBYTE) cbuf,1, tlr+r+h-1, tlc+c, 0);
    cbuf[0] = boxchars[frame][1];
    VioWrtNCell((PBYTE) cbuf, w-2, tlr+r+h-1, tlc+c+1, 0);
    cbuf[0] = boxchars[frame][5];
    VioWrtNCell((PBYTE) cbuf, 1, tlr+r+h-1, tlc+c+w-1, 0);
#ifndef NOZEQ
    zEQ.showmouse();
#endif
}
#endif

#ifdef Cscreen10

// Do nothing constructor for virtual base
zRectangle::zRectangle() {}

// The actual constructor
zRectangle::zRectangle(INT16 r, INT16 c, INT16 h, INT16 w)
{
    tlr = r; tlc = c;
    brr = tlr+h-1; brc = tlc+w-1;
    maxr = h-1;     maxc = w-1;
}

// The constructor has defaulted arguments which give a full 25x80 screen
// area.  The arguments r, c give the position of the top left corner,
// h, w give the height and width, a the current attribute,
// ha the highlight attribute.  Argument su indicates whether or not
// to save the screen under the area - non-zero to save.  If the underlying
// area is saved the new zScreenArea is automatically cleared.
zScreenArea::zScreenArea(INT16 r, INT16 c, INT16 h,
            INT16 w, const char *al, INT16 su)
: zRectangle(r,c,h,w)
{
    init(al, su);
}

// The constructor which takes the zScreenSpec reference argument does
// almost exactly the same.
zScreenArea::zScreenArea(const zScreenSpec &a, INT16 su) 
: zRectangle(a.row,a.col,a.high,a.wide)
{
    init(a.atl, su);
}

void zScreenArea::init(const char *al, INT16 su)
{
    cat = normatt = al[1];
    highatt = al[2];
    saveunder = su;
    coff = cr = cc = 0;
    if (!disp_inited)
        disp_open();
    cbuf[0] = ' '; cbuf[1] = cat;
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
// Mouse cursor must be enabled if you need it
    if (su) {
        VioGetCurPos((PUSHORT) &oldrow, (PUSHORT) &oldcol, 0);
        unsigned cells = wide();
        sb = new unsigned INT16[cells*high()];
        if (!sb) {
            err = ENOMEM;
            saveunder = 0;
            return;
        }
        cells *= 2;
        int h = tlr+high();
        unsigned INT16 *tp = sb;
        for (int i = tlr; i < h; tp += cells/2, ++i)
            VioReadCellStr((PCH) tp, (PUSHORT) &cells,i,tlc,0);
        VioScrollUp(tlr, tlc, brr, brc, 
                    high(), (PBYTE) cbuf, 0);
    } else sb = 0;
#ifndef NOZEQ
    zEQ.showmouse();	// leave it as found
#endif
    err = 0;
}

// Finally it is the destructors job to restore what was under the
// zScreenArea area before it was created if we specified that it
// was to be saved.
zScreenArea::~zScreenArea()
{
#ifndef NOZEQ
    zEQ.hidemouse();
#endif
    if (saveunder) {
        unsigned INT16 *tp = sb, cells = 2*wide();
        int h = tlr+high();
        for (int i = tlr; i < h; tp += cells/2, ++i)
            VioWrtCellStr((PCH) tp,cells,i,tlc,0);
        VioSetCurPos(oldrow,oldcol,0);
        delete sb;
    }
#ifndef NOZEQ
    zEQ.showmouse();	// leave it as found
#endif
}
#endif
#endif

#ifdef Cscreen1
#ifndef NOZEQ
void zScreenArea::sync_mouse()
{
    msm_setcurpos((tlc+cc)*8, (tlr+cr)*8);
}
#endif
#endif
