#include <stdlib.h>
#include <ztui.hpp>
#include <box.hpp>
#include <notice.hpp>

extern char menu_attributes[];
extern zScreenSpec ss1, ss2;
extern zMenSysSpec mss1;
extern zScrollSpec vss1;
const char dba[] = { '\1', '\x17', '\x70' };

zBitVec16 sa = 0b11111;
zBitVec16 sb = 0b11100;

void help()
{
    zNotice h(12,40,menu_attributes,
"This is the help box, all\n\
you provide is a simple text\n\
string with embedded newline\n\
characters laid out like this.\n\
\n\
Press a key to continue");
    h.wait();
}

const char uba[] = { 0,0x31,0x27 };

main()
{
// Declare the TUI with rudimentary user assistance
    zTUI tui(ss1,ss2,mss1,
            "Press F1 for Help",&vss1,0);

    zBox uib(3,4,18,8,uba,1);
    zBox ub(6,41,12,30,uba,1);
    zEvent e;
    int prompt = 1, v, mbs, n = 0;
    tui.us()->cursoff();
    tui.setvsb(50);

// Now process those events
    zEQ.showmouse();
    for (;;) {
        v = tui.source(e,mbs,prompt);
        if (v == ERROR) return -1;
        if (v == IGNORED) {
            prompt = 0;
            if (e.is() == keyboard) {
                if (e.value() == KEY_F1) {
                    help();
                    continue;
                }
// Application keyboard processing here
                int k = e.value();
                if (k & 0xff)
                    ub.putch(k);
                else 
                    ub.printf("\n%04x\n",e.value());
            } else if (e.is() == mouse) {
                if (e.value() == MOUSE_leftdn) {
                    prompt = 1;
// Application mouse processing here etc
                    if (e.value() != MOUSE_move)
                        ub.printf("%d\n",e.value());
                }
            }
            continue;
        }

// Change the active options for one of the menus
        tui.set_allowed(1,(1 & n++)? &sa: &sb);

// Display the value we got from the TUI
        uib.printf("%04x\n",v);

        if (v == 0x0107) {

// Exit was selected - check the users intentions
            tui.us()->cursor();
            zDecision *p = new zDecision(12,20,
                "Are you sure?",dba);
            if (p->choice()) {
                delete p;
                break;
            }
            delete p;
        }
// Need to reset the prompt
        prompt = 1;
    }
    return EXIT_SUCCESS;
}
