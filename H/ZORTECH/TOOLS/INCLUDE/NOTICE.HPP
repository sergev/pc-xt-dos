#ifndef __NOTICE_HPP
#define __NOTICE_HPP
#include <tools.hpp>
#ifndef __OS2__
#include <event.hpp>
#endif
#include <screen.hpp>
#include <iobject.hpp>

class zNotice : private zScreenArea,
            public zInteractiveObject {
public:

// The constructor just needs to be told where to put the notice,
// what attributes to use, and what text to put in it.  It determines
// the required size by counting the number of newline characters in
// the supplied text, and calculating the longest line length.  It is
// up to the user to place the notice so that it will fit properly on
// the screen.
    zNotice(INT16 r, INT16 c,
            const char *atl,
            const char *notice);
    int process_event(zEvent&, INT16&);

// The wait function allows notices to be used as stand alone objects.
// notice objects overide all other objects anyway.  They will handle
// any mouse click or key press.
    void wait();

    int error() { return zScreenArea::error(); }
private:
    static INT16 howhigh(const char *);
    static INT16 howwide();
    static INT16 maxw;
};
#endif
