#ifndef __SEDIT_HPP
#define __SEDIT_HPP
#include <ctype.h>
#include <bios.h>
#include <string.h>
#include <disp.h>
#include <errno.h>
#include <tools.hpp>

const int WHOKNOWS = -1;

// The string editor is affected by the following key values as well
// as ASCII the characters which are to be entered into the string.
// Where these come from depend on the class virtual function
// command_source.
enum  editing_keys
{
    BS = 0x0e08,
    ESC = 0x011b,
    DEL = 0x5300,
    INS = 0x5200,
    LCUR = 0x4b00,
    RCUR = 0x4d00,
    WLEFT = 0x7300,
    WRIGHT = 0x7400,
    HOME = 0x4700,
    END = 0x4f00,
    RET = 0x1c0d,
    UCUR = 0x4800,
    DCUR = 0x5000,
    CTRLEND = 0x7500,
    CTRLHOME = 0x7700
};

// The edit function has a number of return value possibilities which
// are also enumerated:
enum edit_returns {
    SE_ESC,
    SE_RET,
    SE_CUP,
    SE_CDN,
    SE_ONESHOT
};

class zStringEditor {
public:

// There is one constructor with arguments defaulted to maximum string
//  length 79, and insert mode turned on.
    zStringEditor(int = 79, int = 1);


// There are three irtual functions. The first of these, command_source,
// is defaulted in the base class to the PC keyboard. It uses the bioskey
// function which returns a scan code in the high byte and character in the
// low byte of an integer.
    virtual int command_source(void)
        { return bioskey(0); }

// The second virtual function, filter, defaults to the normal printable
// characters. The char pointer and int arguments are there so that in
// derived classes the filter can be passed the current state of the
// string being edited.
    virtual int filter(char c, char *s, int pos)
        { return isprint(c); }

// The virtual converter function in the base class is a dummy. There
// is no conversion. Again provision is made for the convertor function
// to "see" the string.
    virtual int convertor(int k, char *s, int pos)
        { return k; }

// The edit function takes six arguments, three of which are defaulted.
// The first two determine the row/column at which the string to be
// edited will be placed. The char pointer argument points at the string
// to be edited. The disp argument determines whether the string is
// displayed in its existing form before editing commences. startcol
// positions the cursor at the start of editing.  The last argument
// controls the "lifetime" of the edit function.  If it is non-zero
// then edit will process a single keystroke and return a corresponding
// value.  Otherwise editing will continue until edit gets an ESC,
// RET, UCUR, or DCUR keystroke.
    int edit(int r, int c, char *string,
            int disp = 0, int startcol = 0,
            int oneshot = 0);

// The following reporting functions give the maximum permitted length,
// the current length of the edited string, and the cursor position
// within the string respectively.
    int maxlength() const { return maxl; }
    int length() const { return howlong; }
    int curpos() const { return where; }

    int error() { int e = err; err = 0; return e; }
    ~zStringEditor() { delete local; }
private:
    int maxl;                   // of string when editing
    int howlong, where;         // reporting variables
    char *local;                // saves a copy of the string as was
    int insert;                 // insert or overtype?
    int err;
};
#endif
