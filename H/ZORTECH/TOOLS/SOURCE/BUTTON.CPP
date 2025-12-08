#include <string.h>
#include <button.hpp>

zButton::zButton(INT16 r, INT16 c,
        const char *message,
        const char *atr,
        INT16 hi, INT16 sv) :
zRectangle(r,c,3,strlen(message)+4),
zScreenArea(0,0,0,0, atr, sv)
{
    if (err)
        return;
    if (!sv) cleareop();
    box(atr[0],0,0,3,wide());
    rowcol(1,2);
    if (hi) hilite();
    puts(message);
    click_count = 0;
    err = 0;
};

int zButton::process_event(zEvent &e, INT16 &mbs)
{
    if (e.is() == timer || e.is() == kbdint)
        return INTERRUPTED;
    if (e.is() == mouse) {
        if (e.value() == MOUSE_rightdn)
            return CANCELLED;
        if (!in(e.y(), e.x()))
            return IGNORED;
        if (e.value() == MOUSE_leftdn) {
            ++click_count;
            return click_count;
        } else
            return IGNORED;
    } else if (e.value() == 0x1b)
        return CANCELLED;
    else 
        return IGNORED;
}

zDecision::zDecision(INT16 r, INT16 c,
        const char *query,
        const char *atr) :
zRectangle(r,c,7,34),
zScreenArea(0,0,0,0, atr, 1)
{
    if (zScreenArea::error()) {
        err = ENOMEM;
        return;
    }
    rowcol(1,2);
    puts(query);
    box(atr[0],0,0,7,34);
    unsigned char t[3];
    yb = new zButton(r+3,c+6,"OK",atr,1,0);
    if (!yb) { err = ENOMEM; return; }
    nb = new zButton(r+3,c+15,"CANCEL",atr,0,0);
    if (!nb) { err = ENOMEM; return; }
    err = 0;
}

zDecision::~zDecision()
{
    zEQ.hidemouse();
    if (yb) delete yb;
    if (nb) delete nb;
    zEQ.showmouse();
}

int zDecision::process_event(zEvent &e, INT16 &mbs)
{
    int lmbs = mbs;
    zEvent le = e;
    for (;;) {
        if (le.is() == timer || le.is() == kbdint) {
            mbs = lmbs;
            e = le;
            return INTERRUPTED;
        } else if (le.is() == keyboard) {
            int key = le.value();
            if (key == 0x0d)
                return 1;
            else if ((key | 0x20) == 'y')
                return 1;
            else if (key == '\x1b')
                return CANCELLED;
            else
                return HANDLED;
        } else if (le.is() == mouse) {
            if (le.value() == MOUSE_rightdn)
                return HANDLED;
            int rv = yb->process_event(le,0);
            if (rv > 0) return rv;
            rv = nb->process_event(le,0);
            if (rv != IGNORED) return HANDLED;
        }
        lmbs = zEQ.buttons();
        le = zEQ.get();
    }
}

int zDecision::choice()
{
    int mbs = zEQ.buttons();
    zEvent e = zEQ.get();
    int rv;
    if ((rv = process_event(e,mbs)) > 0)
        return 1;
    else 
        return 0;
}
