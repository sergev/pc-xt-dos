#include <stdio.h>
#include <string.h>
#include <screen.hpp>
#include <menu.hpp>

#ifndef __OS2__

zMenu::zMenu(INT16 row, INT16 col, const char *atlist,
        zBitVec *active_list, const zMenuData *md,
            zScreenArea *comment_area, unsigned INT16 *sb) :
zRectangle(row, col, md->options+2, zMenu::widest(md)+4),
zScreenArea(0,0,0,0, atlist, sb == 0)
{
    if (err)
        return;
    save_buffer = sb;
    zEQ.hidemouse();
    if (sb)
        disp_peekbox((unsigned short *) sb,tlr,tlc,brr,brc);
    zEQ.showmouse();
    nops = md->options;
    ac = 0;
    mdp = md;
    active = active_list;
    map = comment_area;
    passive = atlist[3];
    hotspot = atlist[4];
    barlen = wide()-2;

    for (int i = 1; i <= nops; ++i) {
        rowcol(i, 1);
        if (!active->bit(i-1))
            setattr(passive);
        else {
            normal();
            ++ac;
        }
        putch(' ');
        puts(mdp->oplist[i-1]);
        for (int j = barlen-strlen(mdp->oplist[i-1]); j--;)
            putch(' ');
        spot(i);
    }

    normal();
    box(atlist[0],0,0,high(),wide());
// Highlight first active option
    cop = 1;
    while (cop <= nops && !active->bit(cop-1)) ++cop;
    if (cop <= nops) modify(cop,1);
    _state = 1;
}

zMenu::~zMenu()
{
    if (!map) return;
    map->rowcol(0,0);
    map->cleareop();
    zEQ.hidemouse();
    if (save_buffer)
        disp_pokebox((unsigned short *)save_buffer,tlr,tlc,brr,brc);
    zEQ.showmouse();
}

INT16 zMenu::widest(const zMenuData *md)
{
    int n = 0;
    for (int i = 0; i < md->options; ++i) {
        int l = strlen(md->oplist[i]);
        if (l > n)
            n = l;
    }
    return n;
}

void zMenu::modify(INT16 op, INT16 direction)
{
    rowcol(op,1);
    chat(barlen, direction? hiis(): lois());
    spot(op);
    if (map) {
        map->rowcol(0,0);
        map->cleareol();
        map->puts(mdp->comments[cop-1]);
    }
}

void zMenu::spot(INT16 op)
{
    if (mdp->hotkeys && active->bit(op-1)) {
        int hs = mdp->hotkeys[op-1];
        rowcol(op,2+hs/256);
        chat(1,hotspot);
    }
}

int zMenu::hot(char c)
{
    if (mdp->hotkeys) {
        for (int i = 0; i < nops; ++i)
            if ((mdp->hotkeys[i] & 0xff) == c)
                return i;
    }
    return -1;
}

int zMenu::process_event(zEvent& e, INT16 &mbs)
{
    if (e.is() == timer || e.is() == kbdint)
        return INTERRUPTED;
    if (e.is() == keyboard) {
        int key = e.value();
        switch (key) {
        case 0x0d:
            if (!ac)
                return 0;
            return cop;
        case 0x1b:
            return CANCELLED;
        case 0x4800:
        case 0x5000:
            if (!ac)
                return 0;
            modify(cop);
            if (key == 0x4800) {
                for (;;) {
                    if (cop > 1) --cop;
                    else cop = nops;
                    if (active->bit(cop-1))
                        break;
                }
            } else  {
                for (;;) {
                    if (cop < nops) ++cop;
                    else cop = 1;
                    if (active->bit(cop-1))
                        break;
                }
            }
            modify(cop,1);
            return 0;
        default:
            char *p, c = toupper(key & 0xff);
            int i;
            if ((i = hot(c)) != -1) {
                // If recognized change to that option
                if (!active->bit(i))
                    goto ignore;
                ++i;
                modify(cop);
                cop = i;
                modify(cop,1);
                return cop;
            } else {
                // Otherwise ignore
ignore:
                return IGNORED;
            }
        }
    } else if (e.is() == mouse) {
        if (e.value() == MOUSE_rightdn) {
                _state = 0;
                return CANCELLED;
        }
        if (in(e.y(),e.x())) {
            int lr, lc;
            localize(e.y(), e.x(), lr, lc);
            if ((lr >= 1 && lr <= nops) &&
                (lc >= 1 && lc <= barlen)) {
                            // In options area
                if (e.value() == MOUSE_leftdn) {
                    if (!active->bit(lr-1))
                        return 0;
                    modify(cop);
                    cop = lr;
                    modify(cop,1);

// this is best match with CUA, Windows, PM etc, select is
// on mouse button release
                    return 0;
                } else if (e.value() == MOUSE_move && mbs == 1) {
                    if (!active->bit(lr-1))
                        return 0;
                    modify(cop);
                    cop = lr;
                    modify(cop,1);
                    return 0;
                } else if (e.value() == MOUSE_leftup) {
                    if (!active->bit(lr-1))
                        return 0;
                    return cop;
                }
            } else return IGNORED;
        } else {
            if (e.value() == MOUSE_leftdn) {
                _state = 0;
                return CANCELLED;
            }
            return IGNORED;
        }
    }
    return IGNORED;
}

int zMenu::select()
{
    int oldcop, selection;
    for (;;) {
        oldcop = cop;
        int mbs = zEQ.buttons();
        zEvent e = zEQ.get();
        selection = process_event(e,mbs);

// React to a choice, an explicit cancellation, or
// an interrupt event
        if (selection > 0
            || selection == CANCELLED
            || selection == INTERRUPTED)
            return selection;
        if (oldcop != cop && map) {
            map->rowcol(0,0);
            map->cleareol();
            map->puts(mdp->comments[cop-1]);
        }
    }
}

#else
// OS2 version

zMenu::zMenu(INT16 row, INT16 col, const char *atlist,
        zBitVec *active_list, const zMenuData *md,
            zScreenArea *comment_area, unsigned INT16 *sb) :
zRectangle(row, col, md->options+2, zMenu::widest(md)+4),
zScreenArea(0,0,0,0, atlist, sb == 0)
{
    save_buffer = sb;
    zEQ.hidemouse();
    if (sb) {
        int cells = 2*wide();;
        int h = tlr+high();
        unsigned INT16 *tp = sb;
        for (int i = tlr; i < h; tp += cells/2, ++i)
            VioReadCellStr((PCH) tp, (PUSHORT) &cells,i,tlc,0);
    }
    nops = md->options;
    ac = 0;
    mdp = md;
    active = active_list;
    map = comment_area;
    passive = atlist[3];
    hotspot = atlist[4];
    barlen = wide()-2;
    for (int i = 1; i <= nops; ++i) {
        rowcol(i, 1);
        if (!active->bit(i-1))
            setattr(passive);
        else {
            normal();
            ++ac;
        }
        putch(' ');
        puts(mdp->oplist[i-1]);
        for (int j = barlen-strlen(mdp->oplist[i-1]); j--;)
            putch(' ');
        spot(i);
    }
    normal();
    box(atlist[0],0,0,high(),wide());
// Highlight first active option
    cop = 1;
    while (cop <= nops && !active->bit(cop-1)) ++cop;
    if (cop <= nops) modify(cop,1);
    _state = 1;
    zEQ.showmouse();
}

zMenu::~zMenu()
{
    zEQ.hidemouse();
    if (map) {
	    map->rowcol(0,0);
	    map->cleareop();
	}
    if (save_buffer) {
        unsigned INT16 *tp = save_buffer, cells = 2*wide();
        int h = tlr+high();
        for (int i = tlr; i < h; tp += cells/2, ++i)
            VioWrtCellStr((PCH) tp,cells,i,tlc,0);
    }
    zEQ.showmouse();
}

INT16 zMenu::widest(const zMenuData *md)
{
    int n = 0;
    for (int i = 0; i < md->options; ++i) {
        int l = strlen(md->oplist[i]);
        if (l > n)
            n = l;
    }
    return n;
}

void zMenu::modify(INT16 op, INT16 direction)
{
	zEQ.hidemouse();
    rowcol(op,1);
    chat(barlen, direction? hiis(): lois());
    spot(op);
    if (map) {
        map->rowcol(0,0);
        map->cleareol();
        map->puts(mdp->comments[cop-1]);
    }
	zEQ.showmouse();
}

void zMenu::spot(INT16 op)
{
    if (mdp->hotkeys && active->bit(op-1)) {
        int hs = mdp->hotkeys[op-1];
        rowcol(op,2+hs/256);
        chat(1,hotspot);
    }
}

int zMenu::hot(char c)
{
    if (mdp->hotkeys) {
        for (int i = 0; i < nops; ++i)
            if ((mdp->hotkeys[i] & 0xff) == c)
                return i;
    }
    return -1;
}

int zMenu::process_event(zEvent& e, INT16 &mbs)
{
    if (e.is() == timer || e.is() == kbdint)
        return INTERRUPTED;
    if (e.is() == keyboard) {
        int key = e.value();
        switch (key) {
        case 0x0d:
            if (!ac)
                return 0;
            return cop;
        case 0x1b:
            return CANCELLED;
        case 0x4800:
        case 0x5000:
            if (!ac)
                return 0;
            modify(cop);
            if (key == 0x4800) {
                for (;;) {
                    if (cop > 1) --cop;
                    else cop = nops;
                    if (active->bit(cop-1))
                        break;
                }
            } else  {
                for (;;) {
                    if (cop < nops) ++cop;
                    else cop = 1;
                    if (active->bit(cop-1))
                        break;
                }
            }
            modify(cop,1);
            return 0;
        default:
            char *p, c = toupper(key & 0xff);
            int i;
            if ((i = hot(c)) != -1) {
                // If recognized change to that option
                if (!active->bit(i))
                    goto ignore;
                ++i;
                modify(cop);
                cop = i;
                modify(cop,1);
                return cop;
            } else {
                // Otherwise ignore
ignore:
                return IGNORED;
            }
        }
    } else if (e.is() == mouse) {
        if (e.value() == MOUSE_rightdn) {
                _state = 0;
                return CANCELLED;
        }
        if (in(e.y(),e.x())) {
            int lr, lc;
            localize(e.y(), e.x(), lr, lc);
            if ((lr >= 1 && lr <= nops) &&
                (lc >= 1 && lc <= barlen)) {
                            // In options area
                if (e.value() == MOUSE_leftdn) {
                    if (!active->bit(lr-1))
                        return 0;
                    modify(cop);
                    cop = lr;
                    modify(cop,1);
                    return 0;
                } else if (e.value() == MOUSE_move && mbs == 1) {
                    if (!active->bit(lr-1))
                        return 0;
                    modify(cop);
                    cop = lr;
                    modify(cop,1);
                    return 0;
                } else if (e.value() == MOUSE_leftup) {
                    if (!active->bit(lr-1))
                        return 0;
                    return cop;
                }
            } else return IGNORED;
        } else {
            if (e.value() == MOUSE_leftdn) {
                _state = 0;
                return CANCELLED;
            }
            return IGNORED;
        }
    }
    return IGNORED;
}

int zMenu::select()
{
    int oldcop, selection;
    for (;;) {
        oldcop = cop;
        int mbs = zEQ.buttons();
        zEvent e = zEQ.get();
        selection = process_event(e,mbs);

// React to a choice, an explicit cancellation, or
// an interrupt event
        if (selection > 0
            || selection == CANCELLED
            || selection == INTERRUPTED)
            return selection;
        if (oldcop != cop && map) {
            map->rowcol(0,0);
            map->cleareol();
            map->puts(mdp->comments[cop-1]);
        }
    }
}
#endif

