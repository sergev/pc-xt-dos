#ifndef __TUI_HPP
#define __TUI_HPP
#include <tools.hpp>
#include <mensys.hpp>
#include <scroll.hpp>
#include <button.hpp>

class zTUI {
public:

// The constructor needs information describing the main user screen
// area (ss), the message or communications area (ms), the
// menu system (mnu), a text string to give the user some inkling
// of what to do next, and possible specifications for vertical and
// horizontal scroll bars.
    zTUI(const zScreenSpec &ss,
        const zScreenSpec &ms,
        const zMenSysSpec &mnu,
        const char *intro,
        const zScrollSpec *vss = 0,
        const zScrollSpec *hss = 0);

// The source function sits between the application and the event
// queue.  It takes events from the queue, and offers them to the
// various interactive objects comprising the interface.  If none of
// these can process the event,  the event is passed on to the
// application for processing.
    int source(zEvent&, INT16&, INT16 = 1);

// The interface makes the main screen area and the message area
// available to the application via two access functions us,
// which returns a pointer to the main zScreenArea, and ms,
// which returns a pointer to the message area.
    zScreenArea *us() const { return usp; }
    zScreenArea *ms() const { return mesp; }

// The set_allowed function allows the active options list for menu
// mn to be set to list al.
    void set_allowed(INT16 mn, zBitVec *al);

// The setvsb and sethsb functions allow the slider positions
// for the scroll bars to be set,
    void setvsb(INT16);
    void sethsb(INT16);

// and a corresponding pair of functions allow the slider positions
// to be determined.
    int getvsb() const;
    int gethsb() const;

    int error() { int e = err; err = 0; return e; }
    ~zTUI();
private:
    zScreenArea *usp;
    zScreenArea *mesp;
    zMenuSystem *menup;
    zScrollbar *vsbp, *hsbp;
    const char *introp;
    INT16 err;
};
#endif
