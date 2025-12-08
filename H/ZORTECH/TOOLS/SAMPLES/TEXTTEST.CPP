#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <disp.h>
#include <text.hpp>

main()
{
    FILE *fp;
    char buf[82], *p = buf;

//. The text editor would open the display package if we did not, but
//. we will make things prettier by having a box around the text.
    disp_open();
    disp_box(0,0x1f,3,3,21,61);

//. Then declare a ÝzText object with top left corner at 4,4 and bottom
//. right at 20,60, attribute bright white on blue.
    zText ct(4,4,20,60,0x1f);
	disp_flush();

//. If there is a file called test.txt available, then its contents will
//. be read into into our ÝzTextÞ.
    fp = fopen("texttest.txt","r");
    if (fp) {
        while (fgets(buf,80,fp)) {
            p = strchr(buf,'\n');
            *p = '\0';
            if (!ct.addline(buf))
                exit(1);
        }
    }

//. As lines of text were added the current line in Ýct will have
//. progressed away from the start, so now we must go back to the beginning.
    ct.topof();

//. That's all the preliminaries. Now go ahead and edit the text until
//. you are happy with it.  Remember it is ALT+X to get out!
    ct.textedit();

//. Now remove all traces of the existing text from the screen.  This is
//. done quite independantly of the ÝzText object.
    disp_move(0,0);
    disp_setattr(7);
    disp_eeop();

//. The screen is now blank. Move the ÝzText to another position, and
//. redisplay it starting from line 2.
    ct.moveit(2,10);
    disp_box(0,0x1f,1,9,19,67);
    ct.getline(2);
    ct.repaint(0);

//. Then just to show that things are still ok, have a second crack at
//. editing the text.
    int n = ct.textedit();

    disp_setattr(7);
    disp_move(24,0);
    disp_printf(
"Printing %u lines  - any key or Esc to quit",n);
    disp_flush();
    int k = bioskey(0);
    if (k != 0x011b) {

//. Remove successive lines from the beginning of the text and send them
//. to the printer until there are no more.
        line *q;
        while ((q = ct.extract()) != 0) {
            fprintf(stdprn,"%s\n",q->body);
            delete q;
        }
    }
    disp_move(0,0);
    disp_eeop();
    disp_close();
    return EXIT_SUCCESS;
}
