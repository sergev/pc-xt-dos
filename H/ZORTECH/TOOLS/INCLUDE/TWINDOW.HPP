#ifndef WINDOWHPP
#define WINDOWHPP
#include <stdarg.h>
#include <disp.h>
#include <tools.hpp>
#include <sedit.hpp>
#include <dlist.hpp>

class zTWindow {
    friend class zTWindowSet;

public:

// The window constructor takes seven arguments.  It needs a pointer
// to a zTWindowSet, so a window set must be declared before any windows
// can be opened.  The other parameters are the coordinates for the
// window, an integer to specify its attribute/color, and one to determine
// what type of frame it is to have. The frame types are those provided
// by the disp_box function, i.e. double lines, single lines, or solid.
// The last argument governs frame color. If the default is taken this will
// be the same as the rest of the window.
    zTWindow(zTWindowSet *, INT16 tr, INT16 tc,
        INT16 br, INT16 bc, INT16 at, INT16 fr, INT16 fa = -1);

    void clear();

// Change the attribute of n character positions within
// the window to a.
    void chat(INT16 n, INT16 a);

// Put a title in the top frame of the window.
    int title(const char *);

// Change the appearance of a window, that is its attributes and frame style.
    void modify(INT16 att,
            INT16 frame, INT16 fa = -1);

// Hide the window - any which overlap it will also be hidden/suspended.
    void suspend();

// Resuscitate a window - any overlapping windows will be suspended.
    void select();

// Change the active position for the window, that is the position where
// the next output event will occur, and position the cursor there.
    void locate(INT16, INT16);

// putch, puts and printf are with respect to the window. Note that the
// name putch is used because putc would clash with a macro in stdio.h.
    void putch(char c);
    void puts(char *s);
    int printf(char *,...);

// gets uses the string editor class to take input from the cursor
// position within the window.
    int gets(char *s);

// The close function shuts a window down and links it irrevocably
// out of the set.
    int close();

    int error() { int e = err; err = 0; return e; }
    ~zTWindow() { close(); }
private:
    void redisplay();
    void swap();
    void outc(char);

    zTWindowSet *ws;
    zStringEditor se;
    INT16 err;
    char suspended;
    INT16 tlr,tlc,brr,brc;
    INT16 attribute, frameatt;
    INT16 border;
    INT16 wide, high, cr, cc;
    unsigned INT16 *buffer;
};

declare(zGDList,zTWindow);

class zTWindowSet {
    friend class zTWindow;

public:
    zTWindowSet(INT16 tiled) {
        overlapped = tiled;
        if (!disp_inited) disp_open();
    }
    ~zTWindowSet() { purge(); }
private:
    zGDList(zTWindow) wl;
    int add(zTWindow *);
    void purge();

    INT16 overlapped;
};
#endif
