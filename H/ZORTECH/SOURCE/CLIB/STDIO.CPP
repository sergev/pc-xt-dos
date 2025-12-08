#include <string.h>
#include <stdiostr.hpp>

void ostream::flush_stdio()
{
    fflush(stdout);
    fflush(stderr);
}

void ios::sync_with_stdio()
{
    stdiobuf *bp[3];
    memset(bp,0,sizeof(bp));
    bp[0] = new stdiobuf(stdin);
    if (!bp[0]) goto nomem;
    bp[1] = new stdiobuf(stdout);
    if (!bp[1]) goto nomem;
    bp[2] = new stdiobuf(stderr);
    if (!bp[2]) goto nomem;
    cin.set_buffer(bp[0]);
    cout.set_buffer(bp[1]);
    cerr.set_buffer(bp[2]);
    cout.setf(ios::stdio);
    cerr.setf(ios::stdio);
#ifdef __UNIX_
    clog.set_buffer(bp[2]);
    clog.setf(ios::stdio);
#endif
    return;
nomem:
    for (int i = 0; i < sizeof(bp)/sizeof(bp[0]); ++i)
	delete bp[i];
}
