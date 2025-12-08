/*_ bioskey.c */

#ifdef __OS2__

/* OS/2 version of bioskey */

#include <bios.h>

typedef struct _KBDKEY
{	unsigned char	asciichar;
	unsigned char	scancode;
	unsigned char	status;
	unsigned char	reserved;
	unsigned short	shiftstate;
	unsigned long	time;
} KBDKEY;

typedef struct _KBDSTATUS
{	unsigned short size;
	unsigned short status;
	unsigned short turnaround;
	unsigned short interim;
	unsigned short shiftstate;
} KBDSTATUS;

extern _far _pascal KBDGETSTATUS(KBDSTATUS _far *,unsigned short);
extern _far _pascal KBDSETSTATUS(KBDSTATUS _far *,unsigned short);
extern _far _pascal KBDPEEK(KBDKEY _far *,unsigned short);
extern _far _pascal KBDCHARIN(KBDKEY _far *,unsigned short,unsigned short);

unsigned short bioskey(int mode)
{
	return _bios_keybrd(mode);
}

unsigned short _bios_keybrd(int mode)
{
	unsigned short retval = 0;
	unsigned short save;
	KBDSTATUS info;
	KBDKEY key;

	info.size = sizeof(KBDSTATUS);

	KBDGETSTATUS(&info,0);
	save = info.status;
	info.status &= 0xfff7;
	KBDSETSTATUS(&info,0);

	switch (mode) {
		case 0:	KBDCHARIN(&key,0,0);
				retval = key.asciichar + (key.scancode << 8);
				break;
				
		case 1:	KBDPEEK(&key,0);
				if (key.status & 0x40) {
					retval = key.asciichar + (key.scancode << 8);
				} else
					retval = 0;
			break;
				
		case 2: retval = info.shiftstate;
				break;
				
		default: retval = 0;
				break;
	}
	if (mode != 2 && key.asciichar == 224)
		retval &= 0xff00;
	info.status = save;
	KBDSETSTATUS(&info,0);
	return retval;
}

#endif
