#include <stdio.h>
#include <event.hpp>
#include <stdlib.h>

main()
{
    zEQ.set_timer(400);
    for (;;) {
        zEvent e = zEQ.get();
        zEQ.hidemouse();
        printf("%d:%d, %d, %d %04x\n",e.y(),e.x(),e.is(),e.value(),e.shifts());
        zEQ.showmouse();
        if (e.is() == keyboard && e.value() == 0x1b)
            break;
    }
    return EXIT_SUCCESS;
}
