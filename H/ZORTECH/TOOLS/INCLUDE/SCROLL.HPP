#ifndef __SCROLL_HPP
#define __SCROLL_HPP
#include <tools.hpp>
#include <screen.hpp>
#include <iobject.hpp>

// The scroll bars will return values not unlike those returned from
// a menu system. The zMenuSystem definition allowed for 20 main menu
// options so the scroll bar return values need to start at a higher value
// than this. Clicking in the cell at either end of a scroll bar
// will produce either an up or a down signal.  Clicking in the scroll
// bar itself will return a signal in the range 0x2200 to 0x2264
// (0 to 100%) for a vertical bar or 0x2300 to 0x2364 for a horizontal
// one.  The following constants provide for the return values, testing
// etc.
enum zScrollbarConstants {
    VSBUP = 0x2000,
    VSBDOWN,
    HSBUP = 0x2100,
    HSBDOWN,
    VSBMIN = 0x2200,
    VSBMAX = 0x2264,
    HSBMIN = 0x2300,
    HSBMAX = 0x2364
};

//. As usual, a data aggregate is defined to represent the constants
//. which describe a scroll bar.  Determination of the orientation,
//. vertical or horizontal, is done by a constructor argument.
struct zScrollSpec {
    INT16 row, col, len;
    const char *atlp;
};

class zScrollbar : private zScreenArea,
                   public zInteractiveObject {
public:

// The single constructor takes as arguments a reference to a
// zScrollSpec object, and an integer which indicates the orientation
// of the scroll bar, non-zero vertical, zero horizontal, defaulted to
// vertical.  The final argument determines whether the screen under
// the scroll bar should be saved. It is defaulted to not saving.
    zScrollbar(const zScrollSpec &s,
        INT16 orient = 1, INT16 sv = 0);

// Again zScrollBar redefines the virtual process_event function.
    int process_event(zEvent &e, INT16 &mbs);

// Functions set, and percent, deal in percentages. The set
// function puts the scroll bar slider at the best approximation to
// the supplied argument (0 - 100) along the bar.  Remember that we
// are dealing with a text user interface here.  Small scroll bars
// will be quite "notchy".  The percent function returns the present
// position of the slider as an integer in the range 0 - 100.
    void set(INT16 = 0);
    int percent() const;

// The set_delay function controls the repeat rate which obtains when
// the mouse left button is held down on the scroll bar end arrows.
    void set_delay(int d) { delay = d; }

    int error() { return zScreenArea::error(); }
private:
    int handler(zEvent &e, INT16 &mbs);
    void set_abs(INT16);

    INT16 range;
    INT16 orientation;
    INT16 headr, headc;
    INT16 tailr, tailc;
    INT16 current, before;
    INT16 wasputback;
    INT16 delay;
};
#endif
