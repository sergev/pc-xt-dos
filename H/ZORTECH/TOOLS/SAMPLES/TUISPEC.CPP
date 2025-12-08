#include <bios.h>
#include <ztui.hpp>

char *topline[] = {
    "fIrst",
    "Second",
    "Third",
    "Fourth"
};

INT16 tlab[] = { 'I','S','T','F' };

zMenuData mbar = {
    4,
    topline,
    0,
    tlab
};

INT16 initkeys[] = { 0x1700, 0x1f00, 0x1400, 0x2100 };

char *ol1[] = {
    "Cabbages",
    "Kings",
    "Sealing wax",
    "sTring",
    "............",
    "Frogspawn",
    "Exit"
};

INT16 oa1[] = { 'C','K','S',0x100+'T',0,'F','E' };

char *oc1[] = {
    "Here be cabbages",
    "Leaders, Monarchs",
    "Used for sealing letters",
    "For tying up parcels",
    "",
    "In ponds in spring",
    "Quit this session"
};

zMenuData md1 = {
    7,
    ol1,
    oc1,
    oa1
};

char *m2o[] = {
    "Lanthanum",
    "Cerium",
    "Thorium",
    "Uranium",
    "Plutonium"
};

INT16 m2a[] = { 'L','C','T','U','P' };

char *m2c[] = {
    "An actinide element",
    "An actinide element used in gas mantles",
    "Naturally occuring radioactive",
    "Used in the civilian nuclear power program",
    "Used for bombs"
};

zMenuData m2 = {
    5,
    m2o,
    m2c,
    m2a
};

char *m3o[] = {
    "Red",
    "Blue",
    "Green"
};

INT16 m3a[] = { 'R','B','G' };

zMenuData m3 = {
    3,
    m3o,
    m3o,
    m3a
};

char *m4o[] = {
    "Shoes",
    "sOcks",
    "sealinG",
    "Wax"
};

INT16 m4a[] = { 'S',0x100+'O',0x600+'G','W' };

zMenuData m4 = {
    4,
    m4o,
    m4o,
    m4a
};

zMenuData *menus[] = { &md1, &m2, &m3, &m4 };

zBitVec16 seln1 = 0b1101100;
zBitVec16 seln2 = 0b11111;
zBitVec16 seln3 = 0b111;
zBitVec16 seln4 = 0b1011;
zBitVec *accept[] = { &seln1, &seln2, &seln3, &seln4 };

char menu_attributes[] = { '\1', '\x1f', '\x7c', '\x17', '\x7c' };
char scroll_attributes[] = {'\1', '\x1f','\x40' };
char main_screen[] = { '\0','\x30', '\x67' };
char mess_screen[] = { '\0','\7','\x70' };

zMenSysSpec mss1 = {
    &mbar,
    menu_attributes,
    accept,
    menus,
    initkeys
};

zScrollSpec vss1 = {
    1,79,23,
    scroll_attributes
};

zScreenSpec ss1 = {
    1,0,23,79,
    main_screen
};    

zScreenSpec ss2 = {
    24,0,1,80,
    mess_screen
};
