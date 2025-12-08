#include <disp.h>
#include <bios.h>
#include <stdlib.h>
#include <rtc.hpp>

void foo()
{
    zRTClock another(0,72,7);
    disp_move(12,0);
    disp_eeol();
    disp_puts(
"Clock at top right, normal video - press a key\n");
    bioskey(0);
}

main()
{
    zRTClock clock;
    disp_open();
    disp_move(0,0);
    disp_eeop();
    disp_move(12,0);
    disp_puts(
"Clock at bottom right, normal video - press a key\n");
    bioskey(0);
    foo();
    clock.off();
    disp_move(12,0);
    disp_eeol();
    disp_puts(
"Clock turned off - press a key\n");
    bioskey(0);
    clock.on();
    disp_move(12,0);
    disp_eeol();
    disp_puts(
"Clock reverts to the one in main - press a key\n");
    bioskey(0);
    clock.off();
    disp_move(0,0);
    disp_eeop();
    disp_close();
    return EXIT_SUCCESS;
}
