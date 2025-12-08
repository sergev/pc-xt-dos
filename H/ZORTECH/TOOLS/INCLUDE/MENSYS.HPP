#ifndef __MENSYS_HPP
#define __MENSYS_HPP
#include <stdio.h>
#include <bios.h>
#include <msmouse.h>
#include <tools.hpp>
#include <menu.hpp>

enum zMenSysConsts { MAXMENUS = 12 };

// As in the previous cases an aggregate is defined to represent
// the constants which define a menu system.  The first item is a
// pointer to a zMenuData object which describes the options of the
// master menu (the top line) of the menu system.  The second is a
// pointer to the attribute list to be used for the menu system,
// the third points to a list of integer arrays which specify the active
// options for each of the individual drop-down menus.  The fourth
// is a pointer to a set of zMenuData objects, one describing each
// of the drop-down menus.  The final item is a pointer to a list of
// integers representing the keyboard scan codes which define the
// keyboard short-cuts to the individual drop-down menus.
struct zMenSysSpec {
    const zMenuData *topline;
    const char *attribs;
    zBitVec **allowed;
    const zMenuData **menudata;
    const INT16 *initkeys;
};

class zMenuSystem : private zScreenArea,
                   public zInteractiveObject {
public:

// The single constructor takes a reference to a zMenSysSpec object
// as its first argument, and a pointer to a zScreenArea as its second.
// The screen area is used to display the commentaries associated with
// individual menu options, unless a zero argument (the default) is
// supplied, in which case such commentaries are not displayed.
    zMenuSystem(const zMenSysSpec&,
            zScreenArea *comments = 0);

// The zMenuSystem class has its own implementation of the virtual
// process_event function,  and also provides for setting the active
// options list for each of the individual menus.
    int process_event(zEvent &e, INT16 &mbs);

// repaint() was added at the suggestion of Richard Barber to allow
// users to drop into and out of graphics mode.
    void repaint();
    void set_allowed(INT16 mn, zBitVec *);

    int error() { int e = err; err = 0; return e; }
private:
    int handler(zEvent &e, INT16 mbs);
    void modify(int n, int direction = 0);

    zEvent e;
    zMenuData **mdl;
    const char *atl;
    zBitVec **alp;
    zScreenArea *map;
    zMenu *cm;
    INT16 sc[MAXMENUS];
    INT16 len[MAXMENUS];
    INT16 menframe;
    INT16 nops, cop;
    const char **oplist;
    const INT16 *ikp;
    INT16 err;
    unsigned INT16 *save_buffer;
};
#endif
