#include <criterr.hpp>

#ifndef __OS2__

uhp zCEHandler::useptr = 0;
zCEHandler *zCEHandler::list = 0;

extern "C" {
    static int errhdlr(struct INT_DATA *pd)
    {
        int drive = (pd->regs.h.ah & 0x80)? 0: pd->regs.h.al+'A';
        int AX = pd->regs.x.ax;
        pd->regs.h.al = (*zCEHandler::useptr)(drive, AX, pd->regs.x.di);
                        // call user handler with error codes
        return 1;       // immediate RETI
    }
}

zCEHandler::zCEHandler(uhp user_handler)
{
    if (!list)
        err = int_intercept(0x24,errhdlr,256);
    useptr = function = user_handler;
    predecessor = list;
    list = this;
}

void zCEHandler::previous()
{
    if (predecessor) {
        list = list->predecessor;
          useptr = predecessor->function;
    } else {
        useptr = 0;
        int_restore(0x24);
    }
}
#endif
