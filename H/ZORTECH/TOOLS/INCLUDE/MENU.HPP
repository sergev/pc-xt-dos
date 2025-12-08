#ifndef __MENU_HPP
#define __MENU_HPP
#include <stdio.h>
#include <ctype.h>
#ifndef __OS2__
#include <sound.h>
#include <msmouse.h>
#endif
#include <tools.hpp>
#include <bitvec.hpp>
#include <iobject.hpp>
#include <screen.hpp>

// As was the case with the zScreenArea, it is convenient to have an
// aggregate type to describe the parameters of a menu, essentially
// a menu constant.
struct zMenuData {
    INT16 options;
    char **oplist;
    char **comments;
    INT16 *hotkeys;
};

class zMenu : private zScreenArea,
              public zInteractiveObject {
public:

// The constructor needs to specify the position of the menu, its
// attributes, a set of options which are to be initially available,
// and the menu data.  It may also optionally specify a subsidiary
// zScreenArea where comments relating to the current selection can
// be displayed, and a pointer to a buffer where the underlying screen
// data can be stored. A zBitVec of some sort is used to store the
// list of active options
    zMenu(INT16 row, INT16 col, const char *atlist,
        zBitVec *active_list, const zMenuData *md,
        zScreenArea *comment_area = 0,
        unsigned INT16 *sb = 0);

// Aside from the constructor and destructor, there are just three
// public functions.  The first is the implementation of the virtual
// function process_event for class zMenu.  The second, select,
// allows a menu to be used stand alone, so you can just pop one up
// solicit a choice, then get rid of it.  The third allows the
// substitution of a bit vector of active options.
    int process_event(zEvent&, INT16&);
    int select();
    void set_allowed(zBitVec *al) { active = al; }

    int error() { return zScreenArea::error(); }
    ~zMenu();
private:

    static INT16 widest(const zMenuData *);
    void modify(INT16, INT16 = 0);
    void spot(INT16);
    int hot(char);

    INT16 cop, nops;
    const zMenuData *mdp;
    zScreenArea *map;
    INT16 ac, barlen;
    zBitVec *active;
    INT16 passive, hotspot;
    unsigned INT16 *save_buffer;
};
#endif
