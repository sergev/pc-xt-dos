#include <iostream.hpp>
#include <disp.h>
#include <sound.h>
#include <text.hpp>

typedef void (*PFC)(int);
void default_handler(int);
static PFC edit_handler = default_handler;

// The constructor sets up the zGDList(line) to do cleanup of the lines
// as well as the list structure, and sets the line editor to fit the
// editing box.
zText::zText(INT16 r1, INT16 c1,
            INT16 r2, INT16 c2, INT16 att)
: tl(1), le(c2-c1)
{
// Allocate the line buffer,
    lb = (line *) new char[sizeof(INT16)+80];
// and initialise to empty line
    lb->length = 0;
    if (!lb) {
        (*edit_handler)(0);
        exit(1);
    }

    lb->body[0] = '\0';

// Link an empty line into the list.
    if (!linkline()) {
        (*edit_handler)(0);
        exit(1);
    }
// Set window limits etc.
    row = col = tcol = vmove = 0;
    tlr = r1; tlc = c1;
    brr = r2, brc = c2;
    attribute = att;
    disp_setattr(att);
    wide = brc-tlc+1;
    high = brr-tlr+1;

// We have the one (empty) line of text
    cl = maxl = 1;
    nomem = dirty = 0;

    if (!disp_inited) disp_open();
// Clear window, and cursor to start
#ifdef __OS2__
    for (int i = tlr; i <= brr; ++i) {
        disp_move(i,tlc);
        for (int j = wide; j--;) disp_putc(' ');
    }
#else
    disp_scroll(0,tlr,tlc,brr,brc,attribute);
#endif
    disp_move(tlr,tlc);
}

int zText::linkline()
{
    line *p;
// Allocate a line structure just big enough.
    p = (line *) new
        char[sizeof(INT16)+strlen(lb->body)+1];
    if (!p)
        return(0);
    p->length = lb->length;
// Copy line buffer,
    strcpy(p->body,lb->body);
// then link it in after current line.
    if (tl.linkin(p))
        return(0);
    return(1);
}

int zText::replace()
{
    if (dirty) {
// Line buffer needs saving - allocate a structure of correct size.
        line *p = (line *) new
            char[sizeof(INT16)+lb->length+1];
        if (!p)
            return 0;
// Copy line buffer.
        p->length = lb->length;
        strcpy(p->body,lb->body);
        line *q = tl.update(p);
        delete q;
        dirty = 0;
    }
    return 1;
}

void zText::prevlin()
{
// Update list if neccessary
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
// Back one on list,
    tl -= 1;
    --cl;
// Then copy list item into line buffer.
    lb->length = tl()->length;
    strcpy(lb->body,tl()->body);
}

void zText::nextlin()
{
// Update list if neccessary.
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
// Forward one on list,
    tl += 1;
    ++cl;
// thwn copy list item into line buffer.
    lb->length = tl()->length;
    strcpy(lb->body,tl()->body);
}

void zText::up()
{
    if (cl > 1) {
        prevlin();
// Can we stay in same column?
        col = tcol > lb->length?
            lb->length: tcol;
// Must scroll if at top of window
        if (!row) {
            disp_scroll(-1,tlr,tlc,
                    brr,brc,attribute);
            disp_move(tlr,tlc);
// Fill in the current line.
            disp_printf("%s",lb->body);
        } else
// Otherwise just adjust row
            --row;
// Now move to new position.
        disp_move(tlr+row,tlc+col);
        disp_flush();
// Move was up/down, so latch column
        vmove = 1;
    }
}

void zText::down()
{
    if (cl < maxl) {
        nextlin();
        col = tcol > lb->length?
                lb->length: tcol;
        if (row == high-1) {
            disp_scroll(1,tlr,tlc,
                    brr,brc,attribute);
            disp_move(brr,tlc);
            disp_printf("%s",lb->body);
        } else
            ++row;
        disp_move(tlr+row,tlc+col);
        disp_flush();
        vmove = 1;
    }
}

// Enter key pressed - split line at splitcol.
void zText::newline(INT16 splitcol)    
{
    char temp[80], *p;
// Point at the tail,
    p = lb->body+splitcol;
// then copy it into the buffer,
    strcpy(temp,p);

// then cut off its tail - three blind mice ho
    *p = '\0';

// Fill in the new length, and mark the line buffer for update.
    lb->length = strlen(lb->body);
    dirty = 1;

// Replace the line in the list.
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }

// Now get new line in line buffer,
    strcpy(lb->body,temp);
    lb->length = strlen(lb->body);
// and link it in so line buffer == list.
    if (!linkline()) {
        (*edit_handler)(2);
        nomem = 1;
        return;
    }
    dirty = 0;

// Line count and current line both increment.
    ++cl; ++maxl;

// Move cursor to start of tail, and write spaces over it.
    disp_move(tlr+row,tlc+splitcol);
    disp_printf("%*c",wide-splitcol,' ');

// If split line was at bottom of screen scroll up one.
    if (row == high-1)
        disp_scroll(1,tlr,tlc,
            tlr+row,brc,attribute);
    else {
// Otherwise just adjust row,
        ++row;
// then scroll the rest of the area down one
        INT16 d = (row == high-1)? 0: -1;
        disp_scroll(d,tlr+row,tlc,
                brr,brc,attribute);
    }
// Position at start of new line, display it, and update cursor.
    disp_move(tlr+row,tlc);
    disp_printf("%s",lb->body);
    disp_move(tlr+row,tlc);
    disp_flush();
    col = 0;
}

// Cursor at end of line, forward delete joins up current line to next.
void zText::deleolf()
{
    if (cl == maxl) {
// There's nothing there to join
        sound_beep(1000);
        return;
    }
// Otherwise advance list pointer to next line
    tl += 1;
    if (tl()->length+lb->length
                >= wide) {
// Combined line would be loo long, so beep and backtrack.
        sound_beep(1000);
        tl -= 1;
        return;
    }
// Graft next line on to current.
    strcat(lb->body,tl()->body);
    lb->length = strlen(lb->body);
// Link out next line and get rid of it.
    line *p = *tl;
    delete p;
    --maxl;  
// if last line was deleted the currency will have reverted to the current line
    if (cl < maxl)
        tl -= 1;
// Mark the line buffer for update then update the window.
    dirty = 1;
    repaint(row);
}

// Backspace at start of line joins it to previous line,
void zText::deleolb()
{
    if (cl == 1) {
        sound_beep(1000);
        return;
    }
    char temp[81];
    strcpy(temp,lb->body);
// List pointer to previous line
    tl -= 1;
    if (tl()->length+lb->length >= wide) {
// Combined line would be loo long so beep and backtrack
        sound_beep(1000);
        tl += 1;
        return;
    }
    tl += 1;
    line *p = *tl;
    delete p;
// currency will progress if line deleted is not the last
    if (cl < maxl)
        tl -= 1;  
// One less line and one nearer to start of file now.
    --maxl;
    --cl;
    strcpy(lb->body,tl()->body);
    lb->length = tl()->length;
// Adjust column, and row unless at top
    col = lb->length;
    if (row) --row;
// Graft old line on to this one
    strcat(lb->body,temp);
    lb->length = strlen(lb->body);
    dirty = 1;   
    repaint(row);
}

int zText::saveline()
{
    if (!replace()) {
        (*edit_handler)(3);
        return 0;
    }
// Link line out of dlist
    line *p = *tl;
    if (ss.push(p)) {
        (*edit_handler)(3);
        return 0;
    }
    return 1;
}

int zText::copyline()
{
    if (!replace()) {
        (*edit_handler)(4);
        return 0;
    }
    line *p = (line *) new
        char[sizeof(INT16)+lb->length+1];
    if (!p) {
        (*edit_handler)(4);
        return 0;
    }
    p->length = lb->length;
    strcpy(p->body,lb->body);
    if (ss.push(p)) {
        delete p;
        (*edit_handler)(4);
        return 0;
    }
    return 1;
}

void zText::restore()
{
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
    line *p = ss.pop();
    if (!p)
// nothing on stack
        return;
    if (cl > 1) {
        tl -= 1;
        if (tl.linkin(p)) {
            (*edit_handler)(2);
            nomem = 1;
            return;
        }
        ++maxl;
        col = 0;
        repaint(0);
    } else {
        newline(0);
        up();
        lb->length = p->length;
        strcpy(lb->body,p->body);
        dirty = 1;
        delete p;
        disp_printf("%s",lb->body);
        disp_move(tlr,tlc);
        disp_flush();
    }
}

void zText::delline()
{
    if (maxl == 1) {
// First and only line, try and save a copy,
        if (!copyline()) {
            nomem = 1;
            return;
        }
// then zero the existing line.
        lb->length = 0;
        *lb->body = '\0';
        dirty = 1;

// Adjust position.
        row = col = 0;
    } else {
// Keep a copy of the line
        if (!saveline()) {
            nomem = 1;
            return;
        }
// Update the line buffer to reflect the new current line
        lb->length = tl()->length;
        strcpy(lb->body,tl()->body);
        dirty = 0;
        if (cl == maxl) {
            if (row)
                row--;
// Last line - go to end of previous line,
            col = lb->length;
            --cl;
        } else 
// otherwise stick to same column if possible.
            col = tcol > lb->length?
                lb->length: tcol;
        --maxl;
    }
    repaint(row);
}

void zText::goline(unsigned ln)
{
// Ignore if out of range.
    if (ln > maxl || ln < 1)
        return;
// Update list from current line if required
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
// Take the shortest route
    if (maxl-ln < ln-1) {
        tl.end();
        cl = maxl;
        while (cl != ln) {
            --cl;
            tl -= 1;
        }
    } else {
        tl.start();
        cl = 1;
        while (cl != ln) {
            ++cl;
            tl += 1;
        }
    }
// Update the line buffer
    lb->length = tl()->length;
    strcpy(lb->body,tl()->body);
    dirty = 0;
}

// update screen after a goline, line deletion etc, starting at row r
void zText::repaint(INT16 r)
{
    disp_setattr(attribute);
// Remember where we were,
    unsigned savcl = cl;
// then go to the line corresponding to row r.
    goline(cl-(row-r));
// Then put the display position in row r
    disp_move(tlr+r,tlc);
    for (;;) {
        disp_printf("%s%*c",
    tl()->body,wide-tl()->length,' ');
        if (r == high-1)
            break;
        ++r;
        disp_move(tlr+r,tlc);
        if (cl == maxl) {
// Blank rest of window and quit
#ifdef __OS2__
            for (int i = tlr+r; i <= brr; ++i) {
                disp_move(i,tlc);
                for (int j = wide; j--;) disp_putc(' ');
            }
#else
            disp_scroll(0,tlr+r,tlc,
                    brr,brc,attribute);
#endif
            break;
        }
        tl += 1;
        ++cl;   // keep track
    }
// Restore cl and cursor
    goline(savcl);
    disp_move(tlr+row,tlc+col);
    disp_flush();
}

void zText::blockdown()          // down high-1 lines
{
    unsigned dest = cl+high-1;
    if (dest < maxl) {
        goline(dest);
        col = 0;
        repaint(0);
    } else                      // unless end is near
        endof();
}

void zText::blockup()            // up high-1 lines
{
    if (high-1 < cl) {
        unsigned dest = cl-(high-1);
        if (dest-1 < row)
            row = dest-1;
        goline(dest);
        col = 0;
        repaint(0);
    } else
        topof();
}

void zText::topof()              // display first high lines
{
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
    tl.start();                 // we can go directly to start
    lb->length = tl()->length;  // update line buffer
    strcpy(lb->body,tl()->body);
    dirty = 0;
    cl = 1;
    row = col = 0;
    repaint(0);
}

void zText::endof()              // display last high/2 lines
{
    if (!replace()) {
        (*edit_handler)(1);
        nomem = 1;
        return;
    }
    tl.end();
    lb->length = tl()->length;
    strcpy(lb->body,tl()->body);
    dirty = 0;
    col = lb->length;           // position at end
    row = (maxl > (high-1)/2)? (high-1)/2: maxl-1;
    cl = maxl;                  // no more than half way down window
    repaint(0);
}

// This is written for the PC environment - elsewhere the command source
// would have to emulate the PC by producing the following values
// as well as those enumerated in SEDIT.HPP
enum more_keys {
    CTRLBS = 0x0e7f,
    ALTX = 0x2d00,
    PGUP = 0x4900,
    PGDN = 0x5100,
    CTRLPGUP = 0x8400,
    CTRLPGDN = 0x7600,
    ALTR = 0x1300,
    ALTS = 0x1f00
};

int zText::textedit()
{
    char c;
    INT16  kc;
    disp_setattr(attribute);
    disp_move(tlr+row,tlc+col);
    disp_flush();
    for (;;) {
        if (vmove)
            vmove = 0;
        else
            tcol = col;
        c = kc = bioskey(0);
        switch (kc) {
        case LCUR:
        case RCUR:  // pass all these commands to string editor
        case HOME:  // in single command mode, line buffer not affected
        case END:
        case WRIGHT:
        case WLEFT: 
            le.edit(tlr+row,tlc,lb->body,0,col,kc);
            col = le.curpos();
            break;
        case UCUR:
            up();
            break;
        case DCUR:
            down();
            break;
        case BS:
            if (!col)   // backspace at start of line
                deleolb();
            else {      // otherwise give it to string editor
                le.edit(tlr+row,tlc,lb->body,0,col,kc);
                --col;
                --lb->length;
                dirty = 1;  // mark line buffer for update
            }
            break;
        case DEL:
            if (col == lb->length)
                deleolf();  // delete at end of line
            else {      // otherwise give it to string editor
                le.edit(tlr+row,tlc,lb->body,0,col,kc);
                --lb->length;
                dirty = 1;  // mark line buffer for update
            }
            break;
        case INS:
            le.edit(tlr+row,tlc,lb->body,0,col,kc);
            break;
        case RET:       
            newline(col);
            break;
        case CTRLBS:
            delline();
            break;
        case PGUP:
            blockup();
            break;
        case PGDN:
            blockdown();
            break;
        case CTRLPGUP:
            topof();
            break;
        case CTRLPGDN:
            endof();
            break;
        case CTRLEND:
        case CTRLHOME:
            le.edit(tlr+row,tlc,lb->body,0,col,kc);
            dirty = 1;  // mark line buffer for update
            lb->length = strlen(lb->body);
            col = le.curpos();
            break;
        case ALTS:
            copyline();
            if (!nomem && cl < maxl)
                down();
            break;
        case ALTR:
            restore();
            break;
        case ALTX:
            return maxl;    // quit and return number of lines stored
        default:
            if (!c)         // unrecognised function key etc
                sound_beep(1000);
            else            // use line editor to insert it
                le.edit(tlr+row,tlc,lb->body,0,col,kc);
                lb->length = le.length();
                col = le.curpos();
                dirty = 1;  // mark line buffer for update
            break;
        }
        if (nomem)          // memory exhausted drop out and return
            return maxl;    // number of lines
    }
}

int zText::addline(char *s)  // so a list can be built from a file etc
{
    char *p;
    if (strlen(s) > wide) {
        p = s+wide;
        *p = '\0';          // truncate if too long
    }
    strcpy(lb->body,s);     // get in line buffer
    lb->length = strlen(s);
    dirty = 1;
    if (!replace()) {       // update list
        (*edit_handler)(1);
        return 0;
    }
    lb->length = 0;         // re-initialise line buffer
    *lb->body = '\0';
    if (!linkline()) {      // create another list entry
        (*edit_handler)(2);
        return 0;
    }
    dirty = 0;              // line buffer == list
    ++cl; ++maxl;           // keep track and count it
    return 1;
}

line *zText::extract()
{
    if (!tl.size()) return 0;
    tl.start();
    return *tl;
}

char *zText::getline(unsigned n)
{
    goline(n);
    char *p = tl()->body;
    row = col = 0;
    return p;
}

void zText::moveit(INT16 r, INT16 c) // move window - top left corner coordinates
{
    tlr = r; tlc = c; brr = r+high-1, brc = c+wide-1;
}   // do a repaint afterwards to put the text there

char *edit_ermess[] = {
    "no memory to create editor",
    "replace failed, no memory",
    "line not linked, no memory",
    "line not saved, no memory",
    "line not copied, no memory"
};

void default_handler(int n)
{
    disp_move(24,0);
                    // arbitrary choice - configure as required
    disp_printf("edit - %s",edit_ermess[n]);
    sound_beep(1000);
                    // beep so user notices
}

PFC set_edit_handler(PFC nh)
{
    PFC t = edit_handler;
    edit_handler = nh;
    return t;
}
