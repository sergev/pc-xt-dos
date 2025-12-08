#include <scroll.hpp>

zScrollbar::zScrollbar(const zScrollSpec &s,
        INT16 orient, INT16 sv) :
zRectangle(s.row,s.col,orient? s.len: 1,
                orient? 1: s.len),
zScreenArea(0,0,0,0, s.atlp, sv)
{
    if (err)
        return;
    putch(orient? '\x18': '\x1b');
    putch(' ');
    for (int i = s.len-2; i--; putch('\xb0')) ;
    current = before = 1;
    range = s.len-2;
    orientation = orient;
    delay = 300;
    headr = s.row; headc = s.col;
    update(0,0,256*s.atlp[1]+(orient? '\x18': '\x1b'));
    if (orient) {
        update(s.len-1,0,
            256*s.atlp[1]+(orient? '\x19': '\x1a'));
        tailr = s.row+s.len-1;
        tailc = s.col;
    } else {
        update(0,s.len-1,
            256*s.atlp[1]+(orient? '\x19': '\x1a'));
        tailr = s.row;
        tailc = s.col+s.len-1;
    }
    set_abs(0);
    wasputback = err = 0;
};

int zScrollbar::process_event(zEvent &e, INT16 &mbs)
{
    zEvent le = e;
    int rv;
    for (;;) {
        if (le.is() == timer || le.is() == kbdint) {
            rv = INTERRUPTED;
abandon:
            if (_state) {
                _state = 0;
                 set_abs(before);
            }
            break;
        }
        if (le.is() == keyboard) {
            if (le.value() == 0x1b) {
                rv =  CANCELLED;
                goto abandon;
            }
            if (!_state) {
                rv = IGNORED;
                break;
            }
        }
        if (le.value() == MOUSE_rightdn) {
            rv = CANCELLED;
            goto abandon;
        }
        rv = handler(le,mbs);
        if (rv > 0  && rv < 3) {
            if (orientation)
                rv = VSBUP+rv-1;
            else
                rv = HSBUP+rv-1;
            break;
        } else if (!rv) {
            le = zEQ.get();
            continue;
        }
        if (_state) {
            if (rv == 3 || le.value() == MOUSE_leftup) {
                _state = 0;
                if (orientation)
                    rv = VSBMIN+percent();
                else
                    rv = HSBMIN+percent();
                break;
            }
        }
        if (le.is() == mouse) {
            if (le.value() == MOUSE_rightdn)
                rv = CANCELLED;
            else rv = IGNORED;
            break;
        }
    }
    e = le;
    return rv;
}

int zScrollbar::handler(zEvent &e, INT16 &mbs)
{
    int rv = 0, newpos = current;
    int r = e.y(), c = e.x();
    if (wasputback) {
        wasputback = 0;
        zEvent la;
// A previous similar event was potentially perpetuated, wait
// for a decent interval before perpetuating it
        for (int i = delay; i > 0; --i) {
            la = zEQ.lookahead();
            if (la.is() != non_event) break;
        }
        if (i) {
// There has been a subsequent actual event, so we can throw away the
// remembered event, and use the actual one.
            mbs = zEQ.buttons();
            e = zEQ.get();
            r = e.y(), c = e.x();
        }
    }
    if (e.value() == MOUSE_leftdn) {
        if (r == headr && c == headc)
            rv = 1;
        else if (r == tailr && c == tailc)
            rv = 2;
        else if (in(r,c)) {
            before = current;
            _state = 1;
            goto changepos;
        }
        if (rv) {
// It was a mouse click on either the up or down arrows. Remember it
// in case the user just keeps the mouse held down, so the event queue
// will immediately repeat it
            zEQ.putback(e);
            wasputback = 1;            
            return rv;
        }
    }
    if (e.value() == MOUSE_move && _state) {
        if (!in(r,c))
            return 3;
changepos:
        if (orientation) {
            newpos = r-headr-1;
        } else
            newpos = c-headc-1;
        if (newpos < 0) newpos = 0;
        if (newpos > range-1) newpos = range-1;
        set_abs(newpos);
        return 0;
    }
    return -1;
}

void zScrollbar::set_abs(INT16 n)
{
    if (n != current) {
        if (orientation)
            rowcol(current+1,0);
        else
            rowcol(0,current+1);
        putch('\xb0');
        current = n;
        hilite();
        if (orientation)
            rowcol(current+1,0);
        else
            rowcol(0,current+1);
        putch('\xb0');
        normal();
    }
}

void zScrollbar::set(INT16 n)
{
    set_abs(n*(range-1)/100);
}

int zScrollbar::percent() const
{
    return (current*100)/(range-1);
}
