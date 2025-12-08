#include <stddef.h>
#include <twindow.hpp>

#ifndef __OS2__

zTWindow::zTWindow(zTWindowSet *wset,
        INT16 r1, INT16 c1, INT16 r2, INT16 c2,
        INT16 att, INT16 frame, INT16 fa) :
se(c2-c1-1,1)
{
    tlr = r1; tlc = c1; brr = r2;
    brc = c2; attribute = att;
    border = frame;
    wide = brc-tlc-1;
    high = brr-tlr-1; // excluding border
    frameatt = fa == -1? att:  fa;
    size_t bufsize = (r2-r1+1) * (c2-c1+1);
    buffer = new unsigned INT16[bufsize];
    if (!buffer) {
        err = ENOMEM;
        return;
    }

// Save the underlying area
    disp_peekbox((unsigned short *) buffer,r1,c1,r2,c2);

// Then clear the window.,
    disp_scroll(0,r1,c1,r2,c2,att);
// and draw its frame.
    disp_box(frame,frameatt,r1,c1,r2,c2);
// Link this into its set of windows.
    ws = wset;
    ws->add(this);
    suspended = 0;

// Cursor to upper left.
    locate(0,0);
    err = cr = cc = 0;
}

void zTWindow::chat(INT16 ln, INT16 newatt)
{
    unsigned INT16 t[80];
                    // temp area to change attribute
    char *p;
    INT16 i;
    disp_peekbox((unsigned short *) t,ln+tlr,tlc+1,ln+tlr,brc-1);
    for (p = (char *) t+1, i = wide;
                i--; *p = newatt, p += sizeof(unsigned INT16)) ;
    disp_pokebox((unsigned short *) t,ln+tlr,tlc+1,ln+tlr,brc-1);
}

int zTWindow::title(const char *s)
{
    int x, len = strlen(s);
    len = len > wide-4 ? wide-4: len;   // DJB
    x = (wide-len-4)/2;                 // DJB
    disp_move(tlr,tlc+x+2);
    disp_setattr(frameatt);
	disp_putc('[');						// DJB
    for (int i = 0; len--; ++i)			// DJB
    	disp_putc(s[i]);				// DJB
    disp_putc(']');						// DJB
    return len;
}

void zTWindow::redisplay()
{
    int i;

    for (i = 0; i <= high; ++i)
        chat(i,attribute);
    disp_box(border,frameatt,tlr,tlc,brr,brc);
}

void zTWindow::modify(INT16 att,
            INT16 frame, INT16 fa)
{
    frameatt = fa == -1? att: fa;
    attribute = att; border = frame;
    redisplay();
}

void zTWindow::swap()
{
    unsigned INT16 t[80];        // swap buffer
    unsigned INT16 *p;
    int i, w = (wide+2);

    for (i = tlr, p = buffer;
            i <= brr; ++i, p += w) {
        disp_peekbox((unsigned short *) t,i,tlc,i,brc);
        disp_pokebox((unsigned short *) p,i,tlc,i,brc);
        memmove(p,t,w*sizeof(unsigned INT16));
    }
}

void zTWindow::suspend()
{
    if (suspended) return;
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended) {
                (ws->wl)()->swap();
                (ws->wl)()->suspended = 1;
            }
    }
    swap();
    suspended = 1;
}

void zTWindow::select()
{
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended) {
                (ws->wl)()->swap();
                (ws->wl)()->suspended = 1;
            }
        }
    if (suspended) swap();
    suspended = 0;
    locate(cr,cc);
}

void zTWindow::clear()
{
    select();
    if (suspended)
        return;
    disp_scroll(0,tlr+1,tlc+1,
            brr-1,brc-1,attribute);
    locate(0,0);
}

int zTWindow::close()
{
    if (!buffer)
        return 1;   // closed already
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended)
                return 0;
                    // embedded can't close it
    }
    if (!suspended)
        disp_pokebox((unsigned short *) buffer,tlr,tlc,brr,brc);
    delete buffer;
    buffer = 0;     // mark as closed
    *(ws->wl);      // link it out
    return 1;
}

void zTWindow::locate(INT16 r, INT16 c)
{
    if (r < 0 || r > high-1)
        return;
    if (c < 0 || c > wide-1)
        return;
    disp_move(tlr+r+1,tlc+c+1);
    disp_flush();
    cr = r; cc = c;
}

void zTWindow::outc(char c)
{
    if (c < 0 || c >= ' ') {  // DJB
        disp_putc(c);
        if (cc == wide-1) {
            cc = 0;
            goto newline;
        } else
            ++cc;
        disp_move(tlr+cr+1,tlc+cc+1);
    } else {
        switch (c) {
        case '\n':
            cc = 0;
newline:
            if (cr == high-1)
                disp_scroll(1,tlr+1,
            tlc+1,brr-1,brc-1,attribute);
            else
                ++cr;
            break;
        case '\b':
            if (cc)
                --cc;
            break;
        case '\t':
            cc |= 7;
            ++cc;
            if (cc >= wide-1) {
                cc -= wide;
                goto newline;
            }
            break;
        }
    }
    disp_move(tlr+cr+1,tlc+cc+1);
}

void zTWindow::putch(char c)
{
    select();
    disp_setattr(attribute);
    outc(c);
    disp_flush();
}

void zTWindow::puts(char *s)
{
    select();
    disp_setattr(attribute);
    while (*s) outc(*s++);
    disp_flush();
}

int zTWindow::printf(char *format,...)
{
    char buf[256];
    select();
    disp_setattr(attribute);
    va_list arg_ptr;
    va_start(arg_ptr,format);
    int n = vsprintf(buf,format,arg_ptr);
    puts(buf);
    disp_flush();
    return n;
}

int zTWindow::gets(char *s)
{
    select();
    disp_setattr(attribute);
    return se.edit(tlr+cr+1,tlc+cc+1,s,1);
}

int zTWindowSet::add(zTWindow *nw)
{
    wl.end();
    if (wl.linkin(nw) == -1)
        return 0;
    return 1;
}

void zTWindowSet::purge()
{
    zTWindow *p;
    wl.start();
    while (wl.size()) {
        p = *wl;
        p->close();
    }
}

#else
// OS2 version

zTWindow::zTWindow(zTWindowSet *wset,
        INT16 r1, INT16 c1, INT16 r2, INT16 c2,
        INT16 att, INT16 frame, INT16 fa) :
se(c2-c1-1,1)
{
    tlr = r1; tlc = c1; brr = r2;
    brc = c2; attribute = att;
    border = frame;
    wide = brc-tlc-1;
    high = brr-tlr-1; // excluding border
    frameatt = fa == -1? att:  fa;
    size_t bufsize = (r2-r1+1) * (c2-c1+1);
    buffer = new unsigned INT16[bufsize];
    if (!buffer) {
        err = ENOMEM;
        return;
    }

// Save the underlying area
    disp_peekbox((unsigned short *) buffer,r1,c1,r2,c2);

// Then clear the window.
    disp_setattr(att);
    for (int i = r1+1; i < r2; ++i) {
        disp_move(i,c1+1);
        for (int j = wide; j--;) disp_putc(' ');
    }
//    disp_scroll(0,r1,c1,r2,c2,att);
// and draw its frame.
    disp_box(frame,frameatt,r1,c1,r2,c2);
    disp_flush();
// Link this into its set of windows.
    ws = wset;
    ws->add(this);
    suspended = 0;

// Cursor to upper left.
    locate(0,0);
    err = cr = cc = 0;
}

void zTWindow::chat(INT16 ln, INT16 newatt)
{
    unsigned INT16 t[80];
                    // temp area to change attribute
    char *p;
    int i;
    disp_peekbox((unsigned short *) t,ln+tlr,tlc+1,ln+tlr,brc-1);
    for (p = (char *) t+1, i = wide;
                i--; *p = newatt, p += sizeof(unsigned INT16)) ;
    disp_pokebox((unsigned short *) t,ln+tlr,tlc+1,ln+tlr,brc-1);
}

int zTWindow::title(const char *s)
{
    char buf[80];
    strncpy(buf,s,wide-2);
    int x = strlen(s);
    x = (wide-x-2)/2;
    disp_move(tlr,tlc+x+1);
    disp_setattr(frameatt);
    disp_printf("[%s]",s);
    return x;
}

void zTWindow::redisplay()
{
    int i;

    for (i = 0; i <= high; ++i)
        chat(i,attribute);
    disp_box(border,frameatt,tlr,tlc,brr,brc);
}

void zTWindow::modify(INT16 att,
            INT16 frame, INT16 fa)
{
    frameatt = fa == -1? att: fa;
    attribute = att; border = frame;
    redisplay();
}

void zTWindow::swap()
{
    unsigned INT16 t[80];        // swap buffer
    unsigned INT16 *p;
    int i, w = (wide+2);

    for (i = tlr, p = buffer;
            i <= brr; ++i, p += w) {
        disp_peekbox((unsigned short *) t,i,tlc,i,brc);
        disp_pokebox((unsigned short *) p,i,tlc,i,brc);
        memmove(p,t,w*sizeof(unsigned INT16));
    }
}

void zTWindow::suspend()
{
    if (suspended) return;
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended) {
                (ws->wl)()->swap();
                (ws->wl)()->suspended = 1;
            }
    }
    swap();
    suspended = 1;
}

void zTWindow::select()
{
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended) {
                (ws->wl)()->swap();
                (ws->wl)()->suspended = 1;
            }
        }
    if (suspended) swap();
    suspended = 0;
    locate(cr,cc);
}

void zTWindow::clear()
{
    select();
    if (suspended)
        return;
    disp_setattr(attribute);
    for (int i = tlr+1; i < brr; ++i) {
        disp_move(i,tlc+1);
        for (int j = wide; j--;) disp_putc(' ');
    }
//    disp_scroll(0,tlr+1,tlc+1,
//            brr-1,brc-1,attribute);
    locate(0,0);
}

int zTWindow::close()
{
    if (!buffer)
        return 1;   // closed already
    if (ws->overlapped) {
        for ((ws->wl).end();
            (long) (ws->wl)() != (long) this;
                                (ws->wl) -= 1)
            if (!(ws->wl)()->suspended)
                return 0;
                    // embedded can't close it
    }
    if (!suspended)
        disp_pokebox((unsigned short *) buffer,tlr,tlc,brr,brc);
    disp_flush();
    delete buffer;
    buffer = 0;     // mark as closed
    *(ws->wl);      // link it out
    return 1;
}

void zTWindow::locate(INT16 r, INT16 c)
{
    if (r < 0 || r > high-1)
        return;
    if (c < 0 || c > wide-1)
        return;
    disp_move(tlr+r+1,tlc+c+1);
    disp_flush();
    cr = r; cc = c;
}

void zTWindow::outc(char c)
{
    if (c >= ' ') {
        disp_putc(c);
        if (cc == wide-1) {
            cc = 0;
            goto newline;
        } else
            ++cc;
        disp_move(tlr+cr+1,tlc+cc+1);
    } else {
        switch (c) {
        case '\n':
            cc = 0;
newline:
            if (cr == high-1)
                disp_scroll(1,tlr+1,
            tlc+1,brr-1,brc-1,attribute);
            else
                ++cr;
            break;
        case '\b':
            if (cc)
                --cc;
            break;
        case '\t':
            cc |= 7;
            ++cc;
            if (cc >= wide-1) {
                cc -= wide;
                goto newline;
            }
            break;
        }
    }
    disp_move(tlr+cr+1,tlc+cc+1);
}

void zTWindow::putch(char c)
{
    select();
    disp_setattr(attribute);
    outc(c);
    disp_flush();
}

void zTWindow::puts(char *s)
{
    select();
    disp_setattr(attribute);
    while (*s) outc(*s++);
    disp_flush();
}

int zTWindow::printf(char *format,...)
{
    char buf[256];
    select();
    disp_setattr(attribute);
    va_list arg_ptr;
    va_start(arg_ptr,format);
    int n = vsprintf(buf,format,arg_ptr);
    puts(buf);
    disp_flush();
    return n;
}

int zTWindow::gets(char *s)
{
    select();
    disp_setattr(attribute);
    return se.edit(tlr+cr+1,tlc+cc+1,s,1);
}

int zTWindowSet::add(zTWindow *nw)
{
    wl.end();
    if (wl.linkin(nw) == -1)
        return 0;
    return 1;
}

void zTWindowSet::purge()
{
    zTWindow *p;
    wl.start();
    while (wl.size()) {
        p = *wl;
        p->close();
    }
}
#endif
