#include <stdio.h>
#include <event.hpp>

#ifndef __OS2__

// Class zEventQueue implementation - DOS version.

// Two interrupt handlers are required,  one to kill DOS control
// break handling,  and one to provide the timer facility.  These need
// to have C linkage,  and are therefore surrounded by an extern "C"
// declaration.

static volatile int intflag;
static volatile unsigned long countdown;

extern "C"
{                   // Interrupt handlers need C linkage

// The control break handler resets the biosbreak flag,  logs the interrupt,
// then does an immediate return from interrupt.
    static int ctrl_break_handler(INT_DATA *id)
    {
        int bb = 0;
#ifdef DOS386
        ppoke(0x471,&bb,1);
#else
        poke(0x40,0x71,&bb,1);
#endif
        intflag = 1;
        return 1;   // Don't chain the DOS version
    }

// The timer tick handler just decrements the volatile countdown variable
// then returns zero so that any existing interrupt service routine on
// the timer tick gets it's turn.
    static int timer_tick(INT_DATA *id)
    {
        if (countdown)
            countdown--;
        return 0;   // Chain the existing timer tick
    }
}

// A constructor is provided for class event which just initializes all
// of the private variables.  This may be useful for creating fictitious
// events to put back on the event queue.  It uses the private function
// set,  which is also used by class zEventQueue.
zEvent::zEvent(event_t t, coord_t a,
            coord_t b, int v, int sh)
{
    set(t,a,b,v,sh);
}

void zEvent::set(event_t t, coord_t a,
            coord_t b, INT16 v, INT16 sh)
{
    t &= any;
    int tt = t;
    int bitcount = 0;
    for (int i = 4; i--; tt >>= 1)
        bitcount += tt & 1;
    assert(bitcount <= 1);
    type = t;
    cx = a; cy = b;
    val = v;
    shstate = sh;
}

// The constructor for class zEventQueue will fail if an attempt is made
// to create more than one instance of the class.  This would not be
// meaningful,  since there is only one keyboard,  and presumably only one
// mouse, and since DOS is not multitasking!
zEventQueue::zEventQueue()
{
    assert(sentinel == 0);  // Only one event queue.
    counting = 0;
    pushed = 0;
    ++sentinel;
    lastx = lasty = lastbuttons = 0;
    dmode = disp_getmode();
    msm_init();
    msm_showcursor();
    mouse_avail = kbd_avail = timer_avail = int_avail = 0;
    null_event.set(non_event,0,0,0);
    int_intercept(0x1b,ctrl_break_handler,128);
                            // Zap the DOS ^C
}

// The destructor is responsible for restoring any modified interrupt
// vectors.  If it does not get called the results will be spectacularly
// bad,  so preferably make the single zEventQueue an external object,
// and don't use abort().
zEventQueue::~zEventQueue()
{
    msm_term();
    int_restore(0x1b);
    if (counting)
        int_restore(0x1c);
}

// Pending is a private function used by the get and lookahead functions
// to poll for available events.  It uses mevent() to check for mouse
// activity, bioskey(1) to see if there is anything in the keyboard
// buffer, timed out to check if timing is in progress, and whether or
// not it has counted down.  It just looks at inflag to check for control
// break.  Only the events specified by the argument mask value
// are checked,  but this value is defaulted to check all of them.
// Note that the keyboard handling attempts to catch the state of the
// shift keys.  This is largely to maintain interface compatibility
// withe the OS/2 version. It will work if it is actually neccessary
// to wait for a key. It will not work if you have typed ahead, since
// the DOS keyboard handler does not save the shift states in the
// keyboard buffer.
event_t zEventQueue::pending(event_t which)
{
    if (which & mouse) {
        if (pushed & mouse)
            return mouse;
        if (mevent()) {
            mouse_avail = 1;
            return mouse;
        }
    }
    if (which & keyboard) {
        if (pushed & keyboard)
            return keyboard;
        INT16 k, ks = bioskey(2);
        if ((k = bioskey(1)) != 0  && k != -1) {
            bioskey(0);
            k = (k & 0xff)? k & 0xff: k;
            kbd_event.set(keyboard, lastx, lasty, k, ks);
            kbd_avail = 1;
            return keyboard;
        }
    }
    if (which & timer) {
        if (pushed & timer)
            return timer;
        if (timed_out()) {
            counting = 0;
            int_restore(0x1c);
            timer_event.set(timer, lastx, lasty);
            timer_avail = 1;
            return timer;
        }
    }
    if (which & kbdint) {
        if (pushed & kbdint)
            return kbdint;
        if (intflag) {
            bioskey(0);
            // remove dummy entry from kbd buffer
            intflag = 0;
            int_event.set(kbdint, lastx, lasty);
            int_avail = 1;
            return kbdint;
        }
    }
    return non_event;
}

// Get first checks if anything has been put back on the queue.  If
// something has,  then a reference to the pushed event is returned.
// Otherwise pending is called until something happens.  The event
// is then set in the local buffer if this is neccessary,  and a
// reference to it is returned.
zEvent& zEventQueue::get(event_t which, int remove)
{
    if (pushed & which) {
        if (remove)
            pushed = 0;
        return pushed_event;
    }
    for (;;) {
        if (mouse_avail && (which & mouse)) {
            mouse_avail = !remove;
            return mouse_event;
        }
        if (kbd_avail && (which & keyboard)) {
            kbd_avail = !remove;
            return kbd_event;
        }
        if (timer_avail && (which & timer)) {
            timer_avail = !remove;
            return timer_event;
        }
        if (int_avail && (which & kbdint)) {
            int_avail = !remove;
            return int_event;
        }
        event_t event;
        while ((event = pending(which)) == non_event)
            if (!remove) return null_event;
    }
}

// Putback just parks the unwanted event in the pushed_event slot.
// and notes what sort it is.  Only one putback is permitted.
int zEventQueue::putback(zEvent& e)
{
    if (pushed)
        return 0;
    pushed = e.is();
    pushed_event = e;
    return -1;
}

//. Flush gets rid of anything pending which matches the mask argument.
void zEventQueue::flush(event_t which)
{
    pushed &= ~which;	    // DJB
    if (which & mouse)
        mouse_avail = 0;    // Forget any previewed mouse event
    if (which & keyboard) {
        kbd_avail = 0;      // Ditto for keyboard
        bdos(0xc,0,0);      // Flush typeahead buffer
    }
    if (which & timer) {
        timer_avail = 0;
        if (counting)
            int_restore(0x1c);  // cancel any timeout
        counting = countdown = 0;
    }
    if (which & kbdint)
        int_avail = 0;
}

// Mevent compares the previous state of the mouse to it's state
// now,  and constructs a corresponding event if anything has
// changed.
int zEventQueue::mevent()
{
    unsigned buttons,changes;
    coord_t x, y;

    if ((pushed & mouse) || mouse_avail)
        return 1;   // Mouse event already waiting
    buttons = msm_getstatus((unsigned *)&x,(unsigned *)&y);
                    // Retrieve mouse state
    buttons &= 7;   // ignoring all but first 3 buttons
#if GRAPHICS
    y = fg_displaybox[FG_Y2] - y;
                    // Compile it separately if required for graphics use.
#else
    if (dmode == 0 || dmode == 1)
        x /= 16;
    else            // Translate to disp package coordinates (row, col)
        x /= 8;
    y /= 8;
#endif
    if (x != lastx || y != lasty) {
        mouse_event.set(mouse,x,y,MOUSE_move);
        lastx = x;
        lasty = y;
    } else if ((changes = buttons ^ lastbuttons) != 0) {
        int tv;
        if (changes & 1)
            tv = (buttons & 1) ?
                MOUSE_leftdn : MOUSE_leftup;
        else if (changes & 2)
            tv = (buttons & 2) ?
                MOUSE_rightdn : MOUSE_rightup;
        else if (changes & 4)
            tv = (buttons & 4) ?
                MOUSE_leftdn : MOUSE_leftup;
        lastbuttons = buttons;
        mouse_event.set(mouse,x,y,tv);
    } else
        return 0;       // Nothing has changed
    return 1;           // Have a mouse event
}

// Set timer and timed_out support timer events.  The interrupt is only
// intercepted when it is required.
unsigned long zEventQueue::set_timer(unsigned long ticks)
{
    unsigned long t = countdown;
    countdown = ticks;
    if (!counting) {
        counting = 1;
        int_intercept(0x1c,timer_tick,256);
    }
    return t;
}

int zEventQueue::timed_out()
{
    return counting && !countdown;
}

void zEventQueue::hidemouse()
{
    msm_hidecursor();
}

void zEventQueue::showmouse()
{
    msm_showcursor();
}

#else
// OS2 version


static volatile int zEQ_cc_flag;

void far _pascal ctrl_break_handler(unsigned short, unsigned short)
{
    long dummy;
// Register the CTRL-BREAK
    ++zEQ_cc_flag;
// Make sure this handler stays latched in
    DosSetSigHandler(ctrl_break_handler, (PFNSIGHANDLER FAR *) &dummy,
               (PUSHORT) &dummy, SIGA_ACCEPT, SIG_CTRLBREAK);
}

// A constructor is provided for class event which just initializes all
// of the private variables.  This may be useful for creating fictitious
// events to put back on the event queue.  It uses the private function
// set,  which is also used by class zEventQueue.
zEvent::zEvent(event_t t, coord_t a,
            coord_t b, int v, int sh)
{
    set(t,a,b,v,sh);
}

void zEvent::set(event_t t, coord_t a,
            coord_t b, INT16 v, INT16 sh)
{
    t &= any;
    type = t;
    cx = a; cy = b;
    val = v;
    shstate = sh;
}

// Unlike the event queue implementqtion for MS-DOS, under OS/2
// there can be as many event queues in existence as you like.
// How you use this facility is of course up to you.
zEventQueue::zEventQueue()
{
    counting = 0;
    pushed = 0;
    lastx = lasty = lastbuttons = 0;
    mouse_avail = 0;

// Capture the control break mechanism
    DosSetSigHandler(ctrl_break_handler,
        (PFNSIGHANDLER _far *) &prev_handler,
        (PUSHORT) &prev_action, SIGA_ACCEPT, SIG_CTRLBREAK);


// Now get a keyboard handle,
    KbdOpen((PHKBD) &hkbd);
// and wait for the keyboard focus.
    KbdGetFocus(IO_WAIT,hkbd);

// Remember the existing keyboard state
    KBDINFO ki;
    ki.cb = sizeof(ki);
    KbdGetStatus(&ki,hkbd);
    old_keyboard_state = ki.fsMask;

// Set state to binary, no echo.
    ki.fsMask = 6;
    KbdSetStatus(&ki,hkbd);

// Get a mouse handle
    MouOpen((PSZ) 0, (PHMOU) &hmou);
	unsigned eventmask = 2 | 4 | 8 | 0x10;
	int rv = MouSetEventMask((PUSHORT) &eventmask, hmou);
// Set inverse mouse cursor - this is the most universally visible option -
    static unsigned mcmasks[2] = { 0x77ff, 0x7700 };
    PTRSHAPE mpi;
    mpi.cb = 2; mpi.row = mpi.col = 1; mpi.rowHot = mpi.colHot = 0;
    MouSetPtrShape((PBYTE) &mcmasks,
                &mpi, hmou);
// and then show it.
    hidden = 0;
    MouDrawPtr(hmou);
    PTRLOC ploc;
    MouGetPtrPos((PPTRLOC) &ploc, hmou);
    lastx = ploc.col; lasty = ploc.row;

// Set up the timer semaphore
    DosCreateSem(CSEM_PUBLIC,(PHSYSSEM) &hsem,
          (unsigned char _far *) "\\sem\\zeq.sem");
}

// The destructor is responsible for restoring any modified interrupt
// vectors.  If it does not get called the results will be spectacularly
// bad,  so preferably make the single zEventQueue an external object,
// and don't use abort().
zEventQueue::~zEventQueue()
{
    long dummy;
    DosSetSigHandler(prev_handler, (PFNSIGHANDLER FAR *) &dummy,
            (PUSHORT) &dummy, prev_action, SIG_CTRLBREAK);
    KbdClose(hkbd);
    MouClose(hmou);
    if (counting)
        DosTimerStop(htimer);
    DosCloseSem(hsem);
}

// Pending is a private function used by the get function to poll
// for available events.  It first checks for mouse activity,
// then to see if there is anything in the keyboard buffer,
// then to see if timing is in progress, and whether or not it
// has counted down.  It just looks at zEQ_cc_flag to check for control
// break.  Only the events specified by the argument mask value which
// are checked,  but this value is defaulted to check all of them.
event_t zEventQueue::pending(event_t which)
{
    if (which & mouse) {
        if (pushed & mouse)
            return mouse;
        if (mevent()) {
            mouse_avail = 1;
            return mouse;
        }
    }
    if (which & (keyboard | kbdint)) {
        if (pushed & keyboard)
            return keyboard;
        if (pushed & kbdint)
            return kbdint;
        KBDKEYINFO kbki;
        KbdPeek(&kbki,hkbd);
        if (kbki.fbStatus & 0x40) {
            KbdCharIn(&kbki,IO_WAIT,hkbd);
            if (!(kbki.chChar || kbki.chScan) && zEQ_cc_flag) {
                int_event.set(kbdint,lastx,lasty,0,0);
                zEQ_cc_flag = 0;
                int_avail = 1;
                return kbdint;
            }
            unsigned key = kbki.chChar;
            unsigned shst = kbki.fsState;
            if (key == 0 || key == 0xE0)    // DJB
                key = kbki.chScan << 8;     // DJB
            kbd_event.set(keyboard, lastx, lasty, key, shst);
            kbd_avail = 1;
            return keyboard;
        }
    }

    if (which & timer) {
        if (pushed & timer)
            return timer;
        if (counting && !DosSemWait(hsem,SEM_IMMEDIATE_RETURN)) {
            counting = 0;
            // Semaphore is cleared
            timer_event.set(timer, lastx, lasty, 0);
            timer_avail = 1;
            return timer;
	}
    }

    return non_event;
}

// Get first checks if anything has been put back on the queue.  If
// something has,  then a reference to the pushed event is returned.
// Otherwise pending is called until something happens.  The event
// is then set in the local buffer if this is neccessary,  and a
// reference to it is returned.
zEvent& zEventQueue::get(event_t which, int remove)
{
    if (pushed & which) {
        if (remove)
            pushed = 0;
        return pushed_event;
    }
    for (;;) {
        if (mouse_avail && (which & mouse)) {
            mouse_avail = !remove;
            return mouse_event;
        }
        if (kbd_avail && (which & keyboard)) {
            kbd_avail = !remove;
            return kbd_event;
        }
        if (timer_avail && (which & timer)) {
            timer_avail = !remove;
            return timer_event;
        }
        if (int_avail && (which & kbdint)) {
            int_avail = !remove;
            return int_event;
        }
        event_t event;
        while ((event = pending(which)) == non_event)
            if (!remove) return null_event;
    }
}


// Putback just parks the unwanted event in the pushed_event slot.
// and notes what sort it is.  Only one putback is permitted.
int zEventQueue::putback(zEvent& e)
{
    if (pushed)
        return 0;
    pushed = e.is();
    pushed_event = e;
    return -1;
}

// Flush gets rid of anything pending which matches the mask argument.
void zEventQueue::flush(event_t which)
{
    pushed &= ~which;       // DJB
    if (which & mouse) {
        MouFlushQue(hmou);
        mouse_avail = 0;    // Forget any previewed mouse event
    }
    if (which & keyboard)
        KbdFlushBuffer(hkbd);
    if (which & timer) {    // Reset the timer
        DosTimerStop(htimer);
        DosSemClear(hsem);
        counting = 0;
    }
}

// Mevent compares the previous state of the mouse to it's state
// now,  and constructs a corresponding event if anything has
// changed. No attempt is made to deal with a graphics context.
// It is assumed that such requirements will be dealt with via
// Presentation Manager.
int zEventQueue::mevent()
{
    unsigned buttons,changes;
    coord_t x,y;
    MOUEVENTINFO mevi;
    PTRLOC moup;
    unsigned INT16 flag = 0;

    if ((pushed & mouse) || mouse_avail)
        return 1;   // Mouse event already waiting

    MouReadEventQue(&mevi, (PUSHORT) &flag, hmou);

if (mevi.time) {    // something in the queue
    buttons = 0;
    if (mevi.fs & 6)
        buttons |= 1;
    if (mevi.fs & 0x18)
        buttons |= 2;
    if (mevi.fs & 0x60)
        buttons |= 4;

    if ((changes = buttons ^ lastbuttons) != 0) {
        int tv;
        if (changes & 1)
            tv = (buttons & 1) ?
                MOUSE_leftdn : MOUSE_leftup;
        else if (changes & 2)
            tv = (buttons & 2) ?
                MOUSE_rightdn : MOUSE_rightup;
        else if (changes & 4)
            tv = (buttons & 4) ?
                MOUSE_leftdn : MOUSE_leftup;
        lastbuttons = buttons;
        mouse_event.set(mouse,mevi.col,mevi.row,tv,0);
		return 1;
    }
}
    MouGetPtrPos((PPTRLOC) &moup, hmou);
	    y = moup.row;
	    x = moup.col;
	    if (x != lastx || y != lasty) {
	        mouse_event.set(mouse,x,y,MOUSE_move);
	        lastx = x;
	        lasty = y;
			return 1;
		}
		return 0;
}

unsigned long zEventQueue::set_timer(unsigned long ticks)
{
    unsigned long t = countdown;
    countdown = ticks;
    if (!counting) {
        counting = 1;
        DosSemSet(hsem);
        DosTimerAsync(ticks,hsem,&htimer);
    }
    return (unsigned long) t;
}

void zEventQueue::hidemouse()
{
    if (!hidden) {
        NOPTRRECT nop;
        nop.row = nop.col = 0;
        nop.cRow = 24;
        nop.cCol = 79;
        MouRemovePtr((PNOPTRRECT) &nop, hmou);
    }
    ++hidden;
}

void zEventQueue::showmouse()
{
    if (!hidden)
        return;
    if (--hidden == 0)
        MouDrawPtr(hmou);
}
#endif
