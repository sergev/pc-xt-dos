#ifndef __EVENT_HPP
#define __EVENT_HPP
#include <stdlib.h>
#ifdef __OS2__
extern "C" {
    #define INCL_BASE
    #include <os2.h>
}
#endif
#include <msmouse.h>
#include <bios.h>
#include <assert.h>
#include <string.h>
#include <int.h>
#include <tools.hpp>

#ifdef DOS386
extern "C" zapbda(void);
#endif

// The event queue implementation is written so that it can be compiled
// for either text based or graphics oriented applications.  The version
// in the library is for text based applications.  Rebuild the library
// with GRAPHICS #defined to make a graphics oriented version.
#ifdef GRAPHICS
    #include <fg.h>
#else
    #include <disp.h>
#endif


// Figurative constants are provided for the various possible mouse
// events,  and for the 'special keys' on the keyboard,  that is the
// keys which return a zero character value.
enum mouse_events {
    MOUSE_move = -1,
    MOUSE_leftdn = -2,
    MOUSE_leftup = -3,
    MOUSE_rightdn = -4,
    MOUSE_rightup = -5,
    MOUSE_middledn = -6,
    MOUSE_middleup = -7
};

// Keyboard events
enum keyboard_events {
    KEY_up = 	0x4800,
    KEY_down = 0x5000,
    KEY_left = 0x4B00,
    KEY_right = 0x4D00,
    KEY_pgup = 0x4900,
    KEY_pgdn = 0x5100,
    KEY_home = 0x4700,
    KEY_end = 	0x4F00,
    KEY_insert = 0x5200,
    KEY_del = 	0x5300,
    KEY_ctrlhome = 0x7700,
    KEY_ctrlend = 0x7500,
    KEY_ctrlright = 0x7400,
    KEY_ctrlleft = 0x7300,

    KEY_F1 = 	0x3B00,
    KEY_F2 = 	0x3C00,
    KEY_F3 = 	0x3D00,
    KEY_F4 = 	0x3E00,
    KEY_F5 = 	0x3F00,
    KEY_F6 = 	0x4000,
    KEY_F7 = 	0x4100,
    KEY_F8 = 	0x4200,
    KEY_F9 = 	0x4300,
    KEY_F10 = 	0x4400,

    KEY_ShiftF1 = 0x5400,
    KEY_ShiftF2 = 0x5500,
    KEY_ShiftF3 = 0x5600,
    KEY_ShiftF4 = 0x5700,
    KEY_ShiftF5 = 0x5800,
    KEY_ShiftF6 = 0x5900,
    KEY_ShiftF7 = 0x5A00,
    KEY_ShiftF8 = 0x5B00,
    KEY_ShiftF9 = 0x5C00,
    KEY_ShiftF10 = 0x5D00,

    KEY_CtrlF1 = 0x5E00,
    KEY_CtrlF2 = 0x5F00,
    KEY_CtrlF3 = 0x6000,
    KEY_CtrlF4 = 0x6100,
    KEY_CtrlF5 = 0x6200,
    KEY_CtrlF6 = 0x6300,
    KEY_CtrlF7 = 0x6400,
    KEY_CtrlF8 = 0x6500,
    KEY_CtrlF9 = 0x6600,
    KEY_CtrlF10 = 0x6700,

    KEY_AltF1 = 0x6800,
    KEY_AltF2 = 0x6900,
    KEY_AltF3 = 0x6A00,
    KEY_AltF4 = 0x6B00,
    KEY_AltF5 = 0x6C00,
    KEY_AltF6 = 0x6D00,
    KEY_AltF7 = 0x6E00,
    KEY_AltF8 = 0x6F00,
    KEY_AltF9 = 0x7000,
    KEY_AltF10 = 0x7100,

    KEY_AltA = 0x1E00,
    KEY_AltB = 0x3000,
    KEY_AltC = 0x2E00,
    KEY_AltD = 0x2000,
    KEY_AltE = 0x1200,
    KEY_AltF = 0x2100,
    KEY_AltG = 0x2200,
    KEY_AltH = 0x2300,
    KEY_AltI = 0x1700,
    KEY_AltJ = 0x2400,
    KEY_AltK = 0x2500,
    KEY_AltL = 0x2600,
    KEY_AltM = 0x3200,
    KEY_AltN = 0x3100,
    KEY_AltO = 0x1800,
    KEY_AltP = 0x1900,
    KEY_AltQ = 0x1000,
    KEY_AltR = 0x1300,
    KEY_AltS = 0x1F00,
    KEY_AltT = 0x1400,
    KEY_AltU = 0x1600,
    KEY_AltV = 0x2F00,
    KEY_AltW = 0x1100,
    KEY_AltX = 0x2D00,
    KEY_AltY = 0x1500,
    KEY_AltZ = 0x2C00
};

// Class zEvent also enumerates the types of event with which we are
// concerned,  mouse, keyboard, timer, kbdint,  and a further useful
// constant, any,  which represents all of the events.
typedef int coord_t;

enum event_t {
    non_event = 0,
    mouse = 1,
    keyboard = 2,
    timer = 4,
    kbdint = 8,
    any = 0xff
};

class zEvent {
    friend class zEventQueue;

public:

// There is just one event constructor, which by default just puts
// together a non-event.
    zEvent(event_t = non_event, coord_t  = 0,
            coord_t = 0, int = 0, int = 0);

// The rest of the interface to class zEvent just consists of access
// functions which report the type of the event, the particular value
// of the event and the mouse position when the event happened.
    int is() const { return type; }
    int value() const { return val; }
    int shifts() const { return shstate; }
    coord_t x() const { return cx; }
    coord_t y() const { return cy; }
private:
    void set(event_t, coord_t, coord_t, INT16 = 0, INT16 = 0);
    event_t type;   // type of event
    INT16 val;        // actual event value
    coord_t cx,cy;  // mouse position when event ocurred
    INT16 shstate;    // Keyboard shift state as caught
};

// Class zEventQueue provides the required public functions, get,
// lookahead, putback, flush, set_timer,  and a constructor and destructor.
// The get, lookahead, and flush functions can qualify their operation with
// an optional argument which describes what sort of events to operate on.
// A static data member is used in the non-multitasking case to ensure
// that only one zEventQueue is constructed.
class zEventQueue {
public:

// There is just the one constructor, which takes no arguments.
    zEventQueue();

// The get function waits for an event of specified type.
    zEvent& get(event_t which = any, int remove = 1);

// lookahead allows us to take a look at anything which is pending.
// In the absence of anything else, a static non_event is returned.
    zEvent& lookahead(event_t which = any) { return get(which,0); }

// The putback function parks an event back on the queue, only one
// putback is allowed.
    int putback(zEvent&);

// flush provides for dumping some or all pending events.
    void flush(event_t which = any);

// The event queue class provides the capability to initiate a timer
// event t clock ticks from when the set_timer function is called.
    unsigned long set_timer(unsigned long t);

// Finally the class can report the last recorded state of the mouse
// buttons
    int buttons() const { return lastbuttons; }
    void hidemouse();
    void showmouse();
    ~zEventQueue();

private:
    event_t pending(event_t which = any); // Has some event of interest happened?
    int mevent();           // Function to analyse msm_status
#ifndef __OS2__
    int timed_out();        // Is current timer period expired.
#endif

    char counting;          // Timing in progress
#ifdef __OS2__
    long countdown;         // Last time period set
#else
    unsigned long lastcount;
    static INT16 sentinel;  // Protects against multiple event queues
#endif
    char pushed;
    zEvent pushed_event;    // Last event which was put back
// Private variables for tracking mouse
    coord_t lastx, lasty;
    unsigned INT16 lastbuttons;
#ifdef __OS2__
    char hidden;
    PFNSIGHANDLER prev_handler;
                            // Previous ctrl-break handler
    unsigned INT16 prev_action;
                            // Previous ctrl-break handler action
    HKBD hkbd;
    unsigned INT16 old_keyboard_state;
    HMOU hmou;
    HTIMER htimer;
    HSYSSEM hsem;
#endif

    unsigned char dmode;
    char mouse_avail, kbd_avail,
        timer_avail, int_avail;    // Got a previewed event
// Places to park peeked events.
    zEvent mouse_event, kbd_event, timer_event, int_event;
    static zEvent null_event;
};

// The C++ tools library has a built in event queue called zEQ. If you
// include this header file in your code, the linker will pull it in
// automatically.
extern zEventQueue zEQ;
#endif
