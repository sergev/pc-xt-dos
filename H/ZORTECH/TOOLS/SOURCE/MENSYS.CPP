#include <stdio.h>
#include <string.h>
#include <mensys.hpp>

zMenuSystem::zMenuSystem(const zMenSysSpec &ms,
        zScreenArea *comments)
: zRectangle(0,0,1,80),
zScreenArea(0,0,0,0, ms.attribs, 0)
{
    if (zScreenArea::error()) {
        err = ENOMEM;
        return;
    }
    menframe = int(ms.attribs[3]);
    mdl = ms.menudata;
    atl = ms.attribs;
    alp = ms.allowed;
    map = comments;
    ikp = ms.initkeys;
    cm = 0;
    nops = ms.topline->options;
    oplist = ms.topline->oplist;
    cop = 0;
    sc[0] = 0;
    int t, maxo = 0, maxw = 0;
    for (int i = 0; i < nops; ++i) {
        if (mdl[i]->options > maxo)
            maxo = mdl[i]->options;
        for (int j = 0; j < mdl[i]->options; ++j) {
            if ((t = strlen(mdl[i]->oplist[j])) > maxw)
                maxw = t;
        }
    }
    t = (maxw+4) * (maxo+2);    // biggest menu save buffer
    save_buffer = new unsigned INT16[t];
    if (!save_buffer) {
        err = ENOMEM;
        return;
    }
    for (i = 0; i < nops; ++i) {
        len[i] = strlen(oplist[i]) + 1;
        if (i)
            sc[i] = sc[i-1]+len[i-1];
    }
    repaint();
    err = 0;
}

void zMenuSystem::repaint()
{
    rowcol(0,0);
    for (int i = 0; i < nops; ++i) {
        puts(oplist[i]);
        putch(' ');
    }
    cleareol();
}

void zMenuSystem::set_allowed(INT16 mn, zBitVec *set)
{
    alp[mn] = set;
}

void zMenuSystem::modify(int n, int direction)
{
    rowcol(0,sc[cop]);
    chat(len[cop], direction? hiis(): lois());
}

int zMenuSystem::handler(zEvent &e, INT16 mbs)
{
    if (e.is() == timer || e.is() == kbdint) {
        if (cm) {
            delete cm;
            cm = 0;
            modify(cop);
            _state = 0;
        }
        return INTERRUPTED;
    }
    if (e.is() == keyboard) {
        INT16 key = e.value();
        const INT16 *ip = ikp;
        for (int i = 0; i < nops; ++i) {
            if (key == KEY_F10 || key == ip[i]) {
                if (cm) delete cm;
                modify(cop);
                cop = i;
                modify(cop,1);
                cm = new
    zMenu(1, sc[cop], atl, alp[cop],
            mdl[cop], map, save_buffer);
                if (!cm || cm->error()) err = ENOMEM;
                return HANDLED;
            }
        }
        switch (key) {
        case 0x1b:
            if (cm) {
abandon:
                delete cm;
                cm = 0;
                modify(cop);
                _state = 0;
                return CANCELLED;
            } else
                return IGNORED;
            break;
        case 0x4b00:
        case 0x4d00:
            if (cm) {
                delete cm;
                cm = 0;
            } else return IGNORED;
            modify(cop);
            if (key == 0x4b00) {
                if (cop) --cop;
                else cop = nops-1;
            } else  {
                if (cop < nops-1) ++cop;
                else cop = 0;
            }
            modify(cop,1);
            cm = new
                zMenu(1, sc[cop], atl, alp[cop],
            mdl[cop], map, save_buffer);
            if (!cm || cm->error()) err = ENOMEM;
            return HANDLED;
        default:
            return IGNORED;
        }
    } else if (e.is() == mouse) {
        if (e.value() == MOUSE_rightdn) {
            if (cm)
                goto abandon;
            else return IGNORED;
        } else {
            int lr, lc;
            localize(e.y(), e.x(), lr, lc);
            if (lr != 0)    // Not in top row
                return IGNORED;
            if (e.value() == MOUSE_leftdn ||
                (e.value() == MOUSE_move && mbs == 1)) {
                for (int i = 0; i < nops; ++i)
                    if (lc >= sc[i] && lc < sc[i]+len[i])
                        break;
                if (i >= nops) return HANDLED;
                if (cm) {
                    if (i == cop) return HANDLED;
                    delete cm;
                    cm = 0;
                }
                modify(cop);
                cop = i;
                modify(cop,1);
                cm = new
    zMenu(1, sc[cop], atl, alp[cop],
            mdl[cop], map, save_buffer);
                if (!cm || cm->error()) err = ENOMEM;
                return HANDLED;
            } else return HANDLED;
        }
    } else return IGNORED;
}

int zMenuSystem::process_event(zEvent &e, INT16 &mbs)
{
    int s1, s2, lmbs = mbs;
    zEvent le = e;
    normal();
    for (;;) {
        if ((s1 = handler(le,lmbs)) == CANCELLED
                || s1 == INTERRUPTED)
            return s1;
        else if (s1 == IGNORED) {
// This class could not process the event
            if (cm && err == 0) {
                s2 = cm->process_event(le,lmbs);
                if (s2 > 0) {
                    delete cm;
                    cm = 0;
                    modify(cop);
                    _state = 0;
                    return 256*(cop+1)+s2;
                } else if (s2 == CANCELLED
                        || s2 == INTERRUPTED) {
                    delete cm;
                    cm = 0;
                    modify(cop);
                    _state = 0;
                    e = le; mbs = lmbs;
                    return s2;
                }
            } else {
                if (_state) {
                    _state = 0;
                    e = le; mbs = lmbs;
                }
                return IGNORED;
            }
        } else _state = 1;
        lmbs = zEQ.buttons();
        le = zEQ.get();
    }
}
