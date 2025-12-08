// SCRNTST.CPP
#include <dos.h>
#include <bios.h>
#include <sound.h>
#include <stdlib.h>
#include <screen.hpp>

INT16 logo[] = {
    0x0014,                 // 20 words of packed screen
    0xffff,0x000d,0x072a,   // burst, 13 *'s in attribute 7
    0x704c,0x074f,0x7047,0x074f,  // WORD in alternating attribute
    0xffff,0x000d,0x702a,   // more *'s
    0xffff,0x000d,0x702a,
    0x074c,0x074f,0x0747,0x074f,
    0xffff,0x000d,0x072a,   // same again
};

char atr[] = { '\0','\x7','\x70' };

main()
{
// Define two similar screen areas
    zScreenArea sa(4,4,10,30,atr,1),
            sb(6,40,10,30,atr,1);
    double v = 22.0/7;

// test printf and puts and therefore putch
    sa.puts(
"The quick brown fox jumped over the lazy dog,");
    sb.puts(
"The quick brown fox jumped over the lazy dog,");
    sa.setattr(0x70);
    sb.setattr(0x70);
    sa.printf(" the value of ã is about %f xx",v);
    sb.printf(" the value of ã is about %f xx",v);
    sa.cursor();
    sb.cursor();
// beep means press another key
    sound_beep(1000);
getch();
//    bioskey(0);

    sa.backspace();
    sb.backspace();
    sa.backspace();
    sb.backspace();
    sa.cursor(0,0);
    sb.cursor(0,0);
    sound_beep(1000);
getch();
//    bioskey(0);

    sa.cursoff();
    sb.cursoff();
    sa.setattr(7);
    sb.setattr(7);
    sa.rowcol(9,25);
    sb.rowcol(9,25);
    sa.printf("Blue delphiniums");
    sb.printf("Blue delphiniums");

// Force a scroll by writing beyond bottom right
    sa.update(9,29,256*0x70+'+');
    sb.update(9,29,256*0x70+'+');

// Lower right corner update with no scrolling.
    sound_beep(1000);
getch();
//    bioskey(0);

// Clear half a line,
    sa.rowcol(0,15);
    sb.rowcol(0,15);
    sa.cleareol();
    sb.cleareol();

// then the rest of the area.
    sa.rowcol(1,0);
    sb.rowcol(1,0);
    sa.cleareop();
    sb.cleareop();
    sound_beep(1000);
getch();
//    bioskey(0);

// Test the unpack function.
    sa.unpack(logo);
    sb.unpack(logo);
    sa.hilite();
    sb.hilite();
    sa.rowcol(6,4);
    sb.rowcol(6,4);
    sa.puts("WORD");
    sb.puts("WORD");
    sound_beep(1000);
getch();
//    bioskey(0);

// Change the attribute of a section
    sa.rowcol(6,4);
    sb.rowcol(6,4);
    sa.chat(6,7);
    sb.chat(6,7);
    sa.normal();
    sb.normal();
    sound_beep(1000);
getch();
//    bioskey(0);

// Do some scrolling
    sa.scroll(1);
    sb.scroll(1);
    sound_beep(1000);
getch();
//    bioskey(0);
    sa.scroll();
    sb.scroll();
    sound_beep(1000);
getch();
//    bioskey(0);

// Draw a bow within the area
    sa.box(0,5,3,3,6);
    sb.box(0,5,3,3,6);
    sound_beep(1000);
    getch();
    return EXIT_SUCCESS;
}
