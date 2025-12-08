#include <string.h>
#include <notice.hpp>

zNotice::zNotice(INT16 r, INT16 c,
        const char *atl,
        const char *s) :
zRectangle(r, c, zNotice::howhigh(s),
        zNotice::howwide()),
zScreenArea(0,0,0,0, atl, 1)
{
    if (err) return;
    zEQ.hidemouse();
    box(atl[0],0,0,brr-tlr+1,
            brc-tlc+1);
    ++tlr;
    tlc += 2; --brc;
    maxc -= 3;
    rowcol(0,0);
    puts(s);
    --tlr;
    tlc -= 2; ++brc;
    maxc += 3;
    _state = 1;
    zEQ.showmouse();
}

INT16 zNotice::howhigh(const char *s)
{
    const char *p, *q;
    p = q = s;
    maxw = 0;
    for (int i = 0; (p = strchr(p,'\n')) != 0; ++i) {
        if (p-q > maxw) maxw = p-q;
        p = q = p+1;
    }
    int n = strlen(q);
    if (n > maxw) maxw = n;
    return i+3;
}

INT16 zNotice::howwide()
{
    return zNotice::maxw + 4;
}

int zNotice::process_event(zEvent &e, INT16 &mbs)
{
    if (e.is() == keyboard) return HANDLED;
    if (e.is() == mouse) {
        int ev = e.value();
        if (ev == MOUSE_leftdn || ev == MOUSE_rightdn)
            return HANDLED;
    }
    return IGNORED;
}

void zNotice::wait()
{
    zEvent e;
    zEQ.hidemouse();
    for (;;) {
        int mbs = zEQ.buttons();
        e = zEQ.get();
        if (process_event(e,mbs) == HANDLED) break;
    }
    zEQ.showmouse();
}
