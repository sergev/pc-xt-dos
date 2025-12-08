#include <menu.hpp>
#include <stdio.h>
#include <stdlib.h>

//. First we need to define the list of options to be offered
//. by the menu.
char *ol1[] = {
    "Cabbages",
    "Kings",
    "Sealing wax",
    "sTring",
    "Frogspawn",
    "toad Venom"
};

//. Then we can define the hotkey to be associated with each of the
//. options. The high byte contains the position of the hotkey
//. character in the string, while the low byte contains the actual
//. character.  In the common case where the first letter of the
//. option string is the hotkey, this just resolves to the character.
INT16 oa1[] = { 'C','K','S',0x100+'T','F',0x500+'V' };

//. Option comments are optional, but for demonstration purposes we
//. will have some.  These will be displayed in the message area when
//. the corresponding option is highlighted.
char *oc1[] = {
    "Here grow cabbages",
    "Leaders, Monarchs",
    "Used for sealing letters",
    "For tying up parcels",
    "In ponds in spring",
    "Nasty irritant"
};

//. Given these definitions it is now possible to initialize an instance
//. of ÝzMenuData.
zMenuData md1 = {
    6,
    ol1,
    oc1,
    oa1		// zero here for no hot keys
};

//. Then we need an attribute list for the menu.  The last two items
//. give the attributes for inactive options, and hotkey highlighting
//. respectively.
const char attributes[] = { '\0', '\x1f', '\x7c', '\x17', '\x70' };
zBitVec16 alist = 0b110110;

main()
{
// In this case a message screen area will be required.
    zScreenArea messages(24,0,1,80,attributes,1);

//. Then we can dynamically allocate an instance of ÝzMenu, and use its
//. Ýselect function to choose an option.
    zMenu *m = new zMenu(1,10,attributes,&alist,&md1,&messages);
    int r = m->select();
	zEQ.hidemouse();
    delete m;
    printf("\nResult was %d\n",r);
    return EXIT_SUCCESS;
}
