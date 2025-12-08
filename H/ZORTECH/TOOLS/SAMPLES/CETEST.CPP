#include <stdio.h>
#include <bios.h>
#include <sound.h>
#include <io.h>
#include <stdlib.h>
#include <disp.h>
#include <criterr.hpp>

volatile int errors, drive, lastcode;

//. In this example we use two user defined error handlers in succesion.
//. The first one is used to check the status of the printer. Remove
//. remove the paper from it before you run the test.

void dummy() {} // don't want the first handler function at offset 0
                // under pharlap

int user_handler1(int drive, int ax, int errcode)
{
    ++errors;
    lastcode = errcode;
    return CE_FAIL;
}

//. The second one is used to handle a disk error. Leave the gate
//. open on the drive to provoke this one.
int user_handler2(int drive, int ax, int errcode)
{
// Use direct screen output - avoid DOS operations during interrupt
    char buf[6];
    itoa(errcode,buf,10);
    disp_puts("\nError accessing drive ");
    disp_putc(drive);
    disp_puts(" error ");
    disp_puts(buf);
    disp_putc('\n');
    disp_flush();
    return CE_ABORT;
}

//. The ÝzCEHandler class makes it simple and convenient to install
//. an alternative handler to deal with a particular function, the
//. destructor takes care of the deinstallation over all return
//. paths.
void foo()
{
    zCEHandler for_foo(user_handler2);
    open("a:any.old",0);
}

//. Ýmain has the handler which is expecting to deal with a printer
//. error.
main()
{
    disp_open();
    zCEHandler for_main(user_handler1);

    for (;;) {
        disp_printf("Print or Quit P/Q: ");
        disp_flush();
        char c = bioskey(0);
        if (c == 'Q' || c == 'q')
            break;
        for (;;) {
            disp_puts("Printing\n");
            fprintf(stdprn,
"Turn me off I am wasting paper!\n");
            fflush(stdprn);
//    open("a:any.old",0);
            if (errors) {
//                sound_beep(1000);
//                errors = 0;
                disp_printf("\nPrinter fault %d\n",lastcode);
                break;
            }
        }
    }

    foo();
    disp_puts(
"If we got here there were no fatal errors\n");
    disp_close();
    return EXIT_SUCCESS;
}
