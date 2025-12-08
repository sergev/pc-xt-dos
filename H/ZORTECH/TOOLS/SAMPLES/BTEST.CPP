#include <sound.h>
#include <button.hpp>
#include <stdlib.h>

//. ÝzButton and ÝzDecision objects require the usual attribute list.
//. This time the frame value is significant.
const char alist[] = { '\1', '\x30', '\x7c' };

int go()
{
//. Declare a button then collect clicks in it until it is
//. cancelled by a right button click or pressing the Esc key.
    zButton clickme(4,60,"Clickme",alist,0,1);
    for (;;) {
        zEvent e;
        for (;;) {
            e = zEQ.get();
            if (clickme.process_event(e,0) == CANCELLED)
                break;
        }

//. If the user presses the right mouse button or Esc, a ÝzDecision
//. is created dynamically to check that it is OK to quit.
        zDecision *dp = new zDecision(12,20,
"This will end the session\n\
         is this ok",alist);
        int rv = dp->choice();
        delete dp;
        if (rv == 1) break; // user assented
    }
    zEQ.hidemouse();
    return clickme.clicks();
}

main()
{
    printf("Click count = %d\n", go());
    return EXIT_SUCCESS;
}
