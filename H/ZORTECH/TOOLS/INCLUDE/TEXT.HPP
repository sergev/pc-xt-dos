#ifndef __EDIT_HPP
#define __EDIT_HPP
#include <stdlib.h>
#include <tools.hpp>
#include <sedit.hpp>
#include <slist.hpp>
#include <dlist.hpp>
#include <gstack.hpp>

// We need a simple data structure to represent individual lines of
// text which contains length information so that it is not constantly
// necessary to scan lines to determine their length.
struct line {
    INT16 length;
    char body[1];
};

// Both the general purpose linked list and stack are used, so we declare
// variants for each of these data types.
declare(zGDList,line);
declare(zGStack,line);

class zText {
public:

// The constructor sizes and positions the editing area (top row,
// left col, bottom row, right col), and sets the attribute for
// the text.
    zText(INT16 tr, INT16 lc, INT16 br,
            INT16 rc, INT16 att);

// repaint allows for redisplaying the current text from row n of the
// display area.
    void repaint(INT16 n);

// Th topof function puts the context back to the beginning of the text.
    void topof();

// extract allows lines to be removed systematically from the
// beginning of the text.
    line *extract();

// getline is similar except that the lines are left in situ.
    char *getline(unsigned);

// addline provides the opposite sort of facility. It provides
// for lines to be added to the text.
    int addline(char *);

// The text can be repositioned using the moveit function.  This
// adjusts the target area, repaint must be called to actually put
// the text there!
    void moveit(INT16,INT16);

// Finally the textedit function is the one that actually allows us
// to get on with the job of editing the text.
    int textedit();

    ~zText() { delete lb; }
private:
    int replace();              // update linked list line from line buffer
    int linkline();             // put a new line in the list
    void prevlin();
    void nextlin();
    void up();
    void down();
    void newline(INT16);
    void deleolf();             // delete end of line forwards
    void deleolb();             // delete end of line backwards
    void delline();
    int saveline();             // save deleted line
    int copyline();             // save a copy of the current line
    void restore();             // pop a line off the stored list
    void goline(unsigned);      // move to line no
    void blockup();             // up window height-1 lines
    void blockdown();           // down ditto
    void endof();               // to last line

    unsigned cl, maxl;          // current and last line numbers
    line *lb;                   // pointer to current line buffer
    char nomem;                 // no room for any more
    char dirty;                 // current line buffer modified
    INT16 row, col, tcol, vmove;  // housekeeping data
    INT16 tlr, tlc, brr, brc;   // window data
    INT16 wide, high;
    INT16 attribute;
    zGDList(line) tl;           // linked list of text lines
    zGStack(line) ss;           // stack of saved lines
    zStringEditor le;           // use the existing line editor
};
#endif
