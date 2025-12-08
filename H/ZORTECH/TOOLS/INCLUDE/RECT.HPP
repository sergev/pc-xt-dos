#ifndef __RECT_HPP
#define __RECT_HPP
#include <tools.hpp>

typedef unsigned char byte;

class zRectangle {
public:

    int wide() const { return brc-tlc+1; }
    int high() const { return brr-tlr+1; }
// The in function tells you whether a point that you specify is within
// the rectangle. This function is virtual, so that derived objects are
// free to make different interpretations.
    virtual int in(int tr, int tc) const {
        return ((tr >= tlr && tc >= tlc) &&
                 (tr <= brr && tc <= brc));
    }
// zRectangle can convert a pair of global coordinates into coordinates
// with respect to itself.
    virtual void localize(int r, int c, int &lr, int &lc)
        const { lr = r-tlr; lc = c-tlc; }
protected:

// zRectangle is an abstract base class even though it does not have
// any pure virtual functions.  This notion is reinforced by making
// its constructors protected.  Notice that the class has a default
// constructor to allow it to attach to branches of its derivation
// graph which do not pass it constructor arguments.  In these cases
// the default constructor does nothing.  The other constructor
// establishes the position and size of the rectangle.
    zRectangle();   // do nothing constructor
    zRectangle(INT16 r, INT16 c, INT16 h, INT16 w);

// The position and size information is also available to derived
// classes.
    byte tlr, tlc, brr, brc, maxr, maxc;
};
#endif
