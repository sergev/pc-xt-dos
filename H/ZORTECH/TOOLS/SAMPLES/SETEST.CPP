// SETEST.CPP
#include <disp.h>
#include <stdlib.h>
#include <sedit.hpp>

char toupper(char);

char s[80] = "The quick brown fox";     // string to work on

main()
{
    zStringEditor e;
                    // declare an editor
    disp_open();
                    // display package must be open
    disp_move(0,0);
    disp_eeop();
                    // clear screen
    e.edit(12,0,s,1);
                    // edit it - displaying it first
    disp_move(21,0);
    disp_printf(
"Length was %d\nCursor position was %d\n%s",
            e.length(), e.curpos(), s);
                    // display results
    disp_close();
    return EXIT_SUCCESS;
}
