#include <stdlib.h>
#include <scroll.hpp>

//. As always we require an attribute list, and a data aggregate to
//. describe the scroll bar.
const char attribs[] = { '\1', '\x30', '\7c' };
zScrollSpec ss = { 1, 79, 23, attribs };

main()
{
    zScreenArea s;
    s.cleareop();

//. Then we can declare a vertical scroll bar and exersise it until
//. it is cancelled.
    zScrollbar sb(ss);
    int mbs;
    zEvent e;
    for (;;) {
        mbs = zEQ.buttons();
        e = zEQ.get();
        int rv = sb.process_event(e,mbs);
        if (rv == CANCELLED || rv == INTERRUPTED)
            break;
        if (rv == IGNORED)
            continue;
        s.rowcol(12,12);
        s.printf("%04x",rv);
    }
    return EXIT_SUCCESS;
}
