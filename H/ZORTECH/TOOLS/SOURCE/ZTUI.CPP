#include <ztui.hpp>

#ifndef __OS2__

// The constructor takes references to the main and message screen
// areas, atext string to provide first stage help to the user, and
//. optionally two scroll bar specifications.
zTUI::zTUI(const zScreenSpec &ss,
        const zScreenSpec &ms,
        const zMenSysSpec &mnu,
        const char *intro,
        const zScrollSpec *vss,
        const zScrollSpec *hss)
{
    if (!disp_inited)
        disp_open();
    disp_move(0,0);
    disp_setattr(ss.atl[1]);
    zEQ.hidemouse();
    for (int i = disp_numrows*disp_numcols; i--;)
        disp_putc(' ');
    usp = new zScreenArea(ss, 0);
    if (usp->error()) goto nomem;
    mesp = new zScreenArea(ms, 0);
    if (mesp->error()) goto nomem;
    menup = new zMenuSystem(mnu, mesp);
    if (menup->error()) goto nomem;
    if (vss) {
        vsbp = new
            zScrollbar(*vss,1,0);
        if (vsbp->error()) goto nomem;
    } else vsbp = 0;
    if (hss) {
        hsbp = new
            zScrollbar(*hss,0,0);
        if (hsbp->error()) goto nomem;
    } else hsbp = 0;
    introp = intro;
    mesp->puts(introp);
    mesp->cleareol();
    zEQ.showmouse();
    err = 0;
    return;
nomem:
    err = ENOMEM;
};

// The set_allowed function is used to change the active options set
// for a specified menu.
void zTUI::set_allowed(INT16 mn, zBitVec *set)
{
    menup->set_allowed(mn,set);
}    

zTUI::~zTUI()
{
    zEQ.hidemouse();
    delete usp;
    delete mesp;
    delete menup;
    if (vsbp)
        delete vsbp;
    if (hsbp)
        delete hsbp;
    disp_move(0,0);
    disp_flush();
    disp_setattr(7);
    for (int i = disp_numrows*disp_numcols; i--;)
        disp_putc(' ');
    disp_close();
}

// The source function sits between the event queue and the application,
// filtering out events which just affect the interface. When it returns
// a value,  it will be CANCELLED, INTERRUPTED or IGNORED, from the enumeration
// in iobject.hpp, or a positive value describing a menu selection or
// a scroll bar event. In the ignored case, the event in question wiol
// be passed on to the application via the reference arguments.
int zTUI::source(zEvent &e, INT16 &mbs, INT16 prompt)
{
    int lmbs;
    zEvent le;
    int v;
    for (;;) {
        if (prompt) {
            mesp->rowcol(0,0);
            mesp->cleareol();
            mesp->puts(introp);
            prompt = 0;
        }
        lmbs = zEQ.buttons();
        le = zEQ.get();
        v = menup->process_event(le,lmbs);
        if (v == HANDLED) continue;
        if (v != IGNORED) {
            prompt = 1;
            return v;
        }
        if (vsbp) {
            v = vsbp->process_event(le,lmbs);
            if (v == HANDLED) continue;
            if (v != IGNORED)
                return v;
            if (hsbp) {
                v = hsbp->process_event(le,lmbs);
                if (v == HANDLED) continue;
                if (v != IGNORED)
                    return v;
            }
        }
        e = le; mbs = lmbs;
        return v;
    }
}

void zTUI::setvsb(INT16 n)
{
    if (vsbp)
        vsbp->set(n);
}

void zTUI::sethsb(INT16 n)
{
    if (hsbp)
        hsbp->set(n);
}

int zTUI::getvsb() const
{
    if (vsbp) return vsbp->percent();
    else return -1;
}

int zTUI::gethsb() const
{
    if (hsbp) return hsbp->percent();
    else return -1;
}

#else
// OS2 version

// The constructor takes references to the main and message screen
// areas, atext string to provide first stage help to the user, and
//. optionally two scroll bar specifications.
zTUI::zTUI(const zScreenSpec &ss,
        const zScreenSpec &ms,
        const zMenSysSpec &mnu,
        const char *intro,
        const zScrollSpec *vss,
        const zScrollSpec *hss)
{
    BYTE blank[2]; blank[0] = ' '; blank[1] = ss.atl[1];
    zEQ.hidemouse();
    VioScrollUp(0,0,0xffff,0xffff,0xffff,(PBYTE) blank,0);
    usp = new zScreenArea(ss, 0);
    if (usp->error()) goto nomem;
    mesp = new zScreenArea(ms, 0);
    if (mesp->error()) goto nomem;
    menup = new zMenuSystem(mnu, mesp);
    if (menup->error()) goto nomem;
    if (vss) {
        vsbp = new
            zScrollbar(*vss,1,0);
        if (vsbp->error()) goto nomem;
    } else vsbp = 0;
    if (hss) {
        hsbp = new
            zScrollbar(*hss,0,0);
        if (hsbp->error()) goto nomem;
    } else hsbp = 0;
    introp = intro;
    mesp->puts(introp);
    mesp->cleareol();
    zEQ.showmouse();
    err = 0;
    return;
nomem:
    err = ENOMEM;
};

// The set_allowed function is used to change the active options set
// for a specified menu.
void zTUI::set_allowed(INT16 mn, zBitVec *set)
{
    menup->set_allowed(mn,set);
}    

zTUI::~zTUI()
{
    zEQ.hidemouse();
    unsigned a = usp->lois();
    delete usp;
    delete mesp;
    delete menup;
    if (vsbp) delete vsbp;
    if (hsbp) delete hsbp;
    BYTE blank[2]; blank[0] = ' '; blank[1] = 7;
    VioScrollUp(0,0,0xffff,0xffff,0xffff,(PBYTE) blank,0);
}

// The source function sits between the event queue and the application,
// filtering out events which just affect the interface. When it returns
// a value,  it will CANCELLED, INTERRUPTED or IGNORED, from the enumeration
// in iobject.hpp, or a positive value describing a menu selection or
// a scroll bar event. In the ignored case, the event in question will
// be passed on to the application via the reference arguments.
int zTUI::source(zEvent &e, INT16 &mbs, INT16 prompt)
{
    int lmbs;
    zEvent le;
    int v;
    for (;;) {
        if (prompt) {
            mesp->rowcol(0,0);
            mesp->cleareol();
            mesp->puts(introp);
            prompt = 0;
        }
        lmbs = zEQ.buttons();
        le = zEQ.get();
        v = menup->process_event(le,lmbs);
        if (v == HANDLED) continue;
        if (v != IGNORED) {
            prompt = 1;
            return v;
        }
        if (vsbp) {
            v = vsbp->process_event(le,lmbs);
            if (v == HANDLED) continue;
            if (v != IGNORED)
                return v;
            if (hsbp) {
                v = hsbp->process_event(le,lmbs);
                if (v == HANDLED) continue;
                if (v != IGNORED)
                    return v;
            }
        }
        e = le; mbs = lmbs;
        return v;
    }
}

void zTUI::setvsb(INT16 n)
{
    if (vsbp)
        vsbp->set(n);
}

void zTUI::sethsb(INT16 n)
{
    if (hsbp)
        hsbp->set(n);
}

int zTUI::getvsb() const
{
    if (vsbp) return vsbp->percent();
    else return -1;
}

int zTUI::gethsb() const
{
    if (hsbp) return hsbp->percent();
    else return -1;
}
#endif

