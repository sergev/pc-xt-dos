#include <disp.h>
#include <stdio.h>
#include <sound.h>
#include <string.h>
#include <event.hpp>
#include <stdlib.h>

//. First we need some Mickey Mouse objects to interact with.  We can derive
//. These from a simple Ýbox class.   A Ýbox object has a constructor which
//. draws thebox, an Ýin function to determine if a point is inside the
//. box,  and a virtual Ýprocess_event function.
struct box {
    int tlr, tlc, wide, high;
    box(int r, int c, int w, int h);
    int in(int tr, int tc) {
        return (tr > tlr && tc > tlc) &&
            (tr < tlr+high-1 && tc < tlc+wide-1);
    }
    virtual int process_event(zEvent&, int);
};

box::box(int r, int c, int w, int h)
{
    tlr = r; tlc = c; wide = w; high = h;
//    zEQ.hidemouse();
    disp_box(1,7,tlr,tlc,tlr+h-1,tlc+w-1);
	zEQ.showmouse();
}

//. Ýbox has a Ýprocess_event function which just recognizes a mouse
//. click - that is a left button down - in the box.
int box::process_event(zEvent& e, int mbs)
{
    if (e.is() == mouse) {
        if (!in(e.y(),e.x()))
            return -1;
        if (e.value() == MOUSE_leftdn)
            return 0;
    }
    return -1;
}

//. Now from class Ýbox we can derive class Ýbutton,  which is a box with
//. a prompt in it.  It just has a pointer to some static text,  and a
//. constructor.  The base class Ýprocess_event will do for a ÝbuttonÞ.
class button : public box {
    char *tp;
public:
    button(int r, int c, char *t);
};

button::button(int r, int c, char *t)
    : (r,c,strlen(t)+4,3)
{
    tp = t;
//    zEQ.hidemouse();
    disp_move(r+1,c+2);
    disp_puts(tp);
    zEQ.showmouse();
}    

//. Class Ýladder is a precursor for a menu class.  It has a public constructor
//. and it's own version of the virtual Ýprocess_event function.  It also
//. has a private Ýmodify function which changes the attribute of a rung,
//. by default to normal video,  or if the optional second argument is
//. specified,  to inverse video.
class ladder : public box {
    char *rungtags;
    int active;
protected:
    int crung;
    void modify(int r, int hi = 0);
public:
    ladder(int r, int c, int w, int h);
    int process_event(zEvent&, int);
};

ladder::ladder(int r, int c, int w, int h)
                    : (r,c,w,h)
{
    crung = 0;
    active = 0;
}

//. Ýmodify works by peeking the rung into a buffer,  modifying the
//. attribute bytes,  then poking it back.
void ladder::modify(int r, int hi)
{
    unsigned short buffer[30];
    zEQ.hidemouse();
    disp_peekbox(buffer,tlr+r+1,
            tlc+1,tlr+r+1,tlc+wide-1);
    char *p = (char *) buffer;
    ++p;
    for (int i = wide-2; i--;
            *p = hi? 0x70: 7, p += 2) ;
    disp_pokebox(buffer,tlr+r+1,
            tlc+1,tlr+r+1,tlc+wide-1);
	zEQ.showmouse();
}

//. The Ýprocess_event function for class Ýladder responds to either
//. keyboard or mouse events.  If it's a keyboard event,  and the
//. key is a digit,  then the corresponding rung is highlighted, and
//. the rung number is returned.  If It's a mouse event,  then either
//. the rung highlighting is changed to correspond to a mouse movement
//. or the rung number is returned.
int ladder::process_event(zEvent& e, int mbs)
{
    if (in(e.y(),e.x())) {
        if (!active) {
                // Highlighting not already on - wake it up,
                // first turning the mouse cursor off.
            modify(crung,1);
            active = 1;
        }
        if (e.is() == keyboard &&
                (e.value() & 0xff)) {
                // Keyboard event and it is a character
            char c = e.value();
            if (c >= '0' && c <= '0'+high-3) {
                // If in range change to that rung number
                modify(crung);
                crung = c-'0';
                modify(crung,1);
                return crung;
            } else {
                // Otherwise object
                sound_beep(6400);
                return -2;
            }

//. The second argument to Ýprocess_event provides the state of the mouse
//. buttons prior to this event.  This is used to determine if the left
//. button is down.
        } else if (e.is() == mouse) {
            if (e.value() == MOUSE_leftdn ||
                (e.value() == MOUSE_move && (mbs & 1))) {
                // Left button down - move rung number to mouse row
                modify(crung);
                crung = e.y()-tlr-1;
                modify(crung,1);
                return -1;
            } else if (e.value() == MOUSE_leftup)
                // Left button released - return the rung number.
                return crung;
        }
    } else {
                // Mouse not in the ladder,
        if (active) {
                // Switch highlighting off if it was on.
            modify(crung);
            active = 0;
        }
        return -1;
    }
}

main()
{
#ifdef __OS2__
    puts("This test is not suitable for use under OS2 - try TUIT.EXE");
#else
    disp_open();        // Open display package.
    disp_move(0,0);
	disp_setattr(7);
    disp_eeop();
    disp_flush();
    box(0,0,80,22);     // Draw some boxes.
    box(22,0,80,3);
    box(2,75,4,3);

//. Now we can create a ladder and two buttons, one of these two is
//. used to quit, and the other to set the timer.
    ladder ldr(4,6,12,12);
    button qt(18,60,"Quit");
    button st(18,2,"set timer");

    int tc = 1, mbs;
    for (int n = 0;;) {
        zEvent e;
        disp_move(23,tc);
        disp_flush();

//. Remember the mouse button state, then go for the next event.
        mbs = zEQ.buttons();
        e = zEQ.get();   // Get an event
        if (e.is() == kbdint)

//. If it's the keyboard interrupt then quit in an orderly fashion.
            break;
        if (e.is() == timer) {

//. If it is the timer then make a noise to warn the user then press on.
            sound_beep(1131);
            sound_beep(1600);
            continue;
        }

//. Otherwise pass the event to the Ýladder to see if it can make anything
//. of it.  If the Ýladder Ýprocess_event function returns a number
//. greater than -1, then we have a result, so display it. If it returns
//. -2, then an uninterpretable keystroke was encountered while the
//. mouse cursor was in the Ýladder. If it returns -1, then the Ýladder was
//. unable to handle the event.
        n = ldr.process_event(e,mbs);
        if (n > -1) {
            disp_move(3,76);
            zEQ.hidemouse();
            disp_printf("%2d",n);
            zEQ.showmouse();
                        // Display the result.
            continue;
        } else if (n == -2)
            continue;

//. Now try the event on the quit and set timer buttons. If the quit
//. button can handle it then quit. If the timer button can handle it
//. set the timer.
        if (!qt.process_event(e,mbs))
                        // Pass the event to the qt button,
            break;      // quit if it matches
        if (!st.process_event(e,mbs)) {
                        // Pass the event to the st button,
            zEQ.set_timer(200);
                        // Set a timeout if it matches.
            continue;
        }

//. Otherwise if it's a keyboard event just display the character in
//. the bottom line.
        if (e.is() == keyboard) {
            if (e.value() & 0xff) {
                zEQ.hidemouse();
                if (e.value() == '\b') {
                    if (tc > 1) {
                        disp_puts("\b \b");
                        --tc;
                    }
                } else {
                    disp_putc(e.value());
                    if (tc == 78) {
                        unsigned short buf[77];
                        disp_peekbox(buf,23,2,23,78);
                        disp_pokebox(buf,23,1,23,77);
                        disp_puts("\b ");
                    } else ++tc;
                }
                zEQ.showmouse();
            }
        }
    }
    zEQ.hidemouse();
    disp_move(0,0);
    disp_eeop();
    disp_close();
#endif
    return EXIT_SUCCESS;
}
