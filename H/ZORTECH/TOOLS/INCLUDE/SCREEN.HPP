#ifndef __SCREEN_HPP
#define __SCREEN_HPP

#include <stdio.h>
#ifndef __OS2__
#include <string.h>
#endif
#include <disp.h>
#include <stdarg.h>
#include <errno.h>
#include <rect.hpp>
// which include tools.hpp
#ifndef NOZEQ
#include <event.hpp>
#else
#ifdef __OS2__
extern "C" {
#define INCL_BASE
#include <os2.h>
}
#endif
#include <dos.h>
#endif

#ifdef DOS386
extern "C" unsigned INT16 _getcp();
#endif

// It is convenient to have an aggregate to describe the constants which
// determine the position, size, and attributes of a zScreenArea.
struct zScreenSpec {
    INT16 row, col, high, wide;
    const char *atl;
};

// The zScreenArea class is derived from class zRectangle, which in
// this context we require to behave as a virtual base class. The
// class zInteractiveObject will also be derived from this virtual base,
// and specific interactive objects will in turn be derived from
// class zScreenArea, and class zInteractiveObject.  These derived
// objects are to contain only one instance of the base zRectangle
// class.
class zScreenArea : virtual public zRectangle {
public:

// The first zScreenArea constructor takes five integer argument defining
// the postion and size of the area. A pointer argument points at a list
// of attributes. The length of this list will vary in derivatives of
// class zScreenArea, but it will be of a standardized form.  The
// first byte will represent the type of frame to surround the area,
// the second its normal attribute, and the third its highlight
// attribute.  Succeeding bytes will contain attributes to be interpreted
// specially by derived classes.  The base class zScreenArea does
// not come with a frame, so the first of these is ignored.  The
// final integer argument determines whether the screen under the
// new screen area is to be saved for restoration by the destructor.
// The default initialization is full screen white on black normal
// attribute, inverse wideo highlight, without saving the underlying screen.
//
// The second constructor takes a reference to a zScreenSpec, and
// an integer argument which controls saving of the underlying screen.
    zScreenArea(INT16 r = 0, INT16 c = 0,
        INT16 h = 25, INT16 w = 80,
        const char *atl = "\1\7\x70",
        INT16 s = 0);
    zScreenArea(const zScreenSpec&, INT16 = 0);

    int printf(char *format,...);
    void putch(unsigned char, INT16 = 1);
    void puts(const char *s);
    void backspace();

// The function rowcol changes the active position of the screen
// area, i.e. the position at which the next output event will occur.
// The cursor is not effected.
    void rowcol(INT16, INT16);

    void cleareol();
    void cleareop();

// Four functions are concerned with cursor control.  The first, cursoff
// parks the cursor off the screen, thus hiding it.  The second, cursor
// with no arguments, puts the cursor at the active position for the screen
// area.  The third changes the active position and puts the cursor on
// it, and the last synchronizes the mouse cursor, if any, with the
// active position.
    void cursoff();
    void cursor();
    void cursor(INT16, INT16);
#ifndef NOZEQ
    void sync_mouse();
#endif    

// The next group of functions affect display attibutes.  setattr
// temporarily changes the attribute of normal output, the function
// normal can be used to restore the normal attribute set by the
// constructor.  Function sethigh permanently alters the highlight
// attribute for the screen area.  The pair hilite and normal
// toggle output between normal and highlighted.
    void setattr(INT16);
    void sethigh(INT16);
    void hilite();
    void normal();

// Three access functions allow for interrogation of the normal and
// highlight attributes of the zScreenArea, and the current attribute.
    int lois() { return normatt; }
    int hiis() { return highatt; }
    unsigned attrib() { return cat; }

// The chat function provides for changing the attribute of a
// section of the screen area, n positions are changed to attribute
// a.  Function update provides for the modification of one
// character/attribute pair at a specified coordinate.  The character
// and attribute should be packed into a single word, character in
// the low byte, attribute in the high.
    void chat(INT16 n, INT16 a);
    void update(INT16 r, INT16 c, INT16 charatt);

// The scroll function scrolls the whole area up one line with the
// default argument, or down one line if the argument is non-zero.
// The new line is blanked in the current attribute.
    void scroll(INT16 = 0);

// Text with attributes can be displayed in the area from a compressed
// representation via the unpack function.  The packed form consists
// of a stream of integers, first a count of the number of integers
// in the stream, followed by either single integers representing a
// character and attribute, or triplets consisting of a -1 flag, a
// repeat count, then the attribute/character combination to be repeated
// (packed into a word with attribute in the high byte).
    void unpack(const INT16 *);

// The box function takes five integer arguments. The first argument
// determines the frame type as per the disp_box function in the
// Zortech C library. The other four arguments give the position of
// the top left corner and the height and width of the box to be drawn.
    void box(INT16 f, INT16 r, INT16 c, INT16 h, INT16 w);

    int error() { int e = err; err = 0; return e; }
    virtual ~zScreenArea();
protected:

// Some of the data members of the zScreenArea class will be required
// by derived classes, so they are made protected.
    INT16 cr, cc;            // Active position
    byte cat;               // Current Attribute.
    byte normatt;           // Normal attribute.
    byte highatt;           // Attribute for highlighting.
    unsigned INT16 *sb;     // Pointer to screen save buffer.
    byte err;
private:
    void init(const char*, INT16);
    byte coff;              // Should the cursor be showing?
    byte saveunder;         // Should underlying screen be saved?
    INT16 oldrow, oldcol;
#ifdef __OS2__
    byte cbuf[2];
#endif
};

#endif
