#ifndef __BOX_HPP
#define __BOX_HPP
#include <string.h>
#include <tools.hpp>
#include <screen.hpp>

class zBox : public zScreenArea {
public:

// The zBox constructors are identical to those for zScreenArea.  The
// only difference is that the screen area has a frame drawn round it,
// then operations are restricted to the area within the frame.
    zBox(INT16 r, INT16 c, INT16 h, INT16 w,
            const char *atl, INT16 sv);
    zBox(zScreenSpec &s, INT16 sv);

// One operation is allowed outside the contained area.  A title may
// be inserted into the top frame.  If the title is too long it will
// be truncated;
    void title(const char *);

    ~zBox();
};
#endif
