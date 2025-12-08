#include <stdio.h>
#include <disp.h>
#include <event.hpp>
#include <stdlib.h>

main()
{
#ifdef __OS2__
    puts("This test is not suitable for use under OS2 - try TUIT.EXE");
#else
    disp_open();    // Use the display package
    disp_move(0,0);
    disp_eeop();
    disp_flush();

//. Set up a timer event before we get into the event processing loop.
    zEQ.set_timer(180);
                    // Set up for a timer event
    for (int going = 1; going;) {
        zEvent t = zEQ.get();
                    // Wait for something to happen
        switch (t.is()) {

//. If it is a mouse event, then if its just a move, ignore it.
//. Otherwise it is a button change, and if that happens in the top
//. left corner then quit. Otherwise display a character that signifies
//. mouse event.
        case mouse:
            if (t.value() == MOUSE_move)
                break;
                    // Don't bother to display mouse moves
            disp_putc('\x2');
                    // Register a button change
            if (t.x() == 0 && t.y() == 0)
                going = 0;
            break;
        case keyboard:
//. If it is a keyboard event check if it is the Esc key, and if so
//. break. If not, display a character that signifies keyboard event.
                    // Display the character if not zero
            disp_putc((t.value() & 0xff)?
                    t.value(): '\x3');
            if (t.value() == 0x1b)
                going = 0;
            break;
        case timer:
//. If it is a timer event, display a character to register it, then
//. use putback to put an imaginary keyboard event into the queue, and
//. reset the timer.
            disp_putc('\x19');
                    // Register timer event,
            zEQ.putback(zEvent(keyboard,0,0,0x54));
                    // check out putback,
            zEQ.set_timer(100);
                    // and set the timer again.
            break;
        case kbdint:
            disp_putc('\x4');
            break;
        }
        if (!going)
            break;
//. The second half of the loop does the same sort of things with the
//. Ýlookahead function, with displayed characters in inverse video.
        t = zEQ.lookahead();
        disp_setattr(0x70);
        switch (t.is()) {
        case mouse:
            if (t.value() == MOUSE_move)
                break;
            disp_putc('\x2');
            break;
        case keyboard:
            disp_putc((t.value() & 0xff)? t.value(): '\x3');
            break;
        case timer:
            disp_putc('\x19');
            break;
        case kbdint:
                // Don't bother to display this one.
            break;
        }
        disp_setattr(7);
    }
    disp_move(0,0);
    disp_eeop();
    disp_close();

//. Now use Ýstdout to make sure there is no "^C" waiting in the wings.
    puts("Hello World");
#endif
    return EXIT_SUCCESS;
}
