#include <stdlib.h>
#include <event.hpp>
#include <twindow.hpp>

//. First we need to declare a couple of window sets, with which
//. our actual windows will be associated.  The first set is declared
//. to be overlapped, the second not.
zTWindowSet ws1(1);
zTWindowSet ws2(0);

main()
{
    char buf[30];

//. Now associate some windows with the window sets.  The first three
//. form Ýws1Þ,  the last is an isolated instance.
    zTWindow w1(&ws1,2,2,12,30,0x70,0),
             w2(&ws1,6,15,16,43,0x70,0),
             w3(&ws1,4,24,14,52,0x70,1),
             dialog(&ws2,21,30,23,78,7,2);

// Null string in the input buffer
    *buf = '\0';

// Put a title on the dialog window
    dialog.title("Window with title");

// Then communicate with the user
    dialog.puts("Press a key to suspend w3");
    bioskey(0);

// Suspend each of the windows in ws1 in turn
    w3.suspend();
    dialog.clear();
    dialog.puts("Press a key to suspend w2");
    bioskey(0);
    w2.suspend();
    dialog.clear();
    dialog.puts("Press a key to suspend w1");
    bioskey(0);
    w1.suspend();
    dialog.clear();
    dialog.puts("Press a key");
    bioskey(0);

// Reselect the windows
    w1.select();
    dialog.clear();
    dialog.puts("And again");
    bioskey(0);
    w2.select();
    bioskey(0);
    w3.select();

// Suspend the dialog window
    dialog.suspend();

// Check out the input/ output functions
    w3.puts("Enter some text");
    w3.locate(1,0);
    w3.gets(buf);
    w2.puts(
"This should be too long for the width \
of the window\n\nPress a key");
    bioskey(0);  
    w1.printf("Jack and Gill ran\n");
    w1.printf("up the hill\n");
    w1.printf("to fetch a pail\n");
    w1.printf("of water\n\n");
    w1.printf("Press a key");
    bioskey(0);

// Then change attribute and frame of window 3
    w3.select();
    w3.clear();
    w3.puts("Press a key to\nchange this window");
    bioskey(0);
    w3.modify(7,0);
    w3.locate(5,0);
    w3.printf(
"%d number in a\n window\n\nPress a key",22);
    bioskey(0);

// Calling dialog clear reinstates the dialog window
    dialog.clear();
    dialog.puts("Thats all - key to quit");
    bioskey(0);
    return EXIT_SUCCESS;
}
