#ifndef __BUTTON_HPP
#define __BUTTON_HPP
#include <tools.hpp>
#include <screen.hpp>
#include <iobject.hpp>

class zButton : private zScreenArea,
                public zInteractiveObject {
public:

// The constructor takes arguments specifying the position of the top
// left corner of the button, the text to be displayed in the button,
// the usual attribute list, and two integers which control initial
// highlighting of the button, and saving of the underlying screen.
// the defaults in this case are no highlight and save the underlying screen.
    zButton(INT16 r, INT16 c,
        const char *message,
        const char *atr,
        INT16 hi = 0, INT16 sv = 1);

// Class zButton needs its own implementation of the virtual process_event
// function.
    int process_event(zEvent &e, INT16 &mbs);

// Buttons just count mouse clicks.  Other than the standard
// process_event function they just provide for reporting the number
// of clicks recorded, and resetting the count to zero.
    int clicks() { return click_count; }
    void reset() { click_count = 0; }

// Two functions of the private base class need to be republished,
// so that a zButton can be highlighted, or restored to its normal
// attribute.
    zScreenArea::hilite;
    zScreenArea::normal;

    int error() { return zScreenArea::error(); }
private:
    INT16 click_count;
};

// Class zDecision is defined next,  it is a zScreenArea with two
// associated zButtons, also of course it is a zInteractiveObject.
class zDecision : private zScreenArea,
                 public zInteractiveObject {
public:

// The decision box constructor just need a position for the box,
// box, the question to be displayed in it, and the customary attribute
// list.
    zDecision(INT16 r, INT16 c,
        const char *query,
        const char *atr);
    ~zDecision();

    int process_event(zEvent &e, INT16 &mbs);

// There is also a stand-alone funtion to solicit a decision from a user.
    int choice();

    int error() { int e = err; err = 0; return e; }
private:
    zButton *yb, *nb;
    INT16 err;
};
#endif
