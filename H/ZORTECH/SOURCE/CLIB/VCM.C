/*_ vcm.c   Mon Jan 29 1990   Modified by: Walter Bright */
/* Virtual code manager				*/
/* Copyright (C) 1989-1990 by Walter Bright	*/
/* All Rights Reserved				*/
/* Written by Walter Bright			*/

#if M_I86OM || M_I86VM

#include	<dos.h>
#include	<string.h>
#include	<stdlib.h>
#include	<assert.h>
#include	<io.h>
#include	<int.h>

#define STATIC static
#define ZDB	1

#pragma ZTC align 1			/* no alignment for union GATE	*/

typedef union Gate
{
    struct Callgate
    {
	unsigned char opcode;		/* interrupt instruction (0xCD)	*/
#define GATE_INT	0xCD
	unsigned char intnum;		/* interrupt number		*/
	unsigned char vcmnum;		/* which vseg			*/
	unsigned offset;		/* offset into that vseg	*/
    } cg;
    struct Resident
    {
#define GATE_JMP	0xEA
	unsigned char opcode;		/* FAR JMP (0xEA)		*/
	unsigned offset;
	unsigned segment;
    } r;
    struct Free
    {
	unsigned char type;		/* to indicate this is an f	*/
#define GATE_FREE	0x01
	unsigned char dummy;		/* for alignment		*/
	union Gate *next;		/* for free list		*/
    } f;
} Gate;

#pragma ZTC align

Gate *gate_freelist;	/* start of list of available gates	*/
Gate *gates;			/* array of gates for return gates	*/

/* Data structure at beginning of EXE file	*/
struct header
{
    unsigned short signature;	/* MZ					*/
#define SIGNATURE 0x5A4D
    unsigned short remainder;	/* load module image size % 512		*/
    unsigned short numpages;	/* number of 512 byte pages		*/
    unsigned short numreloc;	/* number of relocation items		*/
    unsigned short imageoffset;	/* offset in paragraphs to load image	*/
    unsigned short minpara;
    unsigned short maxpara;
    unsigned short ssoff;	/* in segment form			*/
    unsigned short spvalue;	/* initial SP value			*/
    unsigned short checksum;
    unsigned short ipvalue;	/* initial IP value			*/
    unsigned short csoff;	/* initial CS offset			*/
    unsigned short reloff;	/* offset of relocation table		*/
    unsigned short overlay;	/* overlay number			*/
};

/*********************************
 * Info we keep in memory about each vseg.
 */

typedef struct Vcm
{
    unsigned long diskoffset;	/* offset to vseg from start of EXE file */
    unsigned size;		/* size of vseg			*/
    unsigned next;
    unsigned prev;		/* threads for next/prev Vcm in LRU list */

    char location;		/* VCM_XXXX				*/
/* Location of vseg	*/
#define VCM_DISK	0
#define VCM_MEMORY	1
    char pad;			/* for alignment			*/

    union U
    {
	struct UDISK
	{
	} udisk;
	struct UMEMORY
	{   unsigned segment;	/* segment of where it's at		*/
	    unsigned offset;	/* needed so we can free() the buffer	*/
	} umemory;
    } loc;
} Vcm;

/* Variables set by BLINK	*/
extern Vcm far vcmtbl[];
extern unsigned __vcmmax;	/* number of vsegs			*/
extern unsigned __vcmmaxsize;	/* size of largest vseg		*/
extern unsigned char _intno;	/* interrupt number that calls vseg service routine */

/* Variable that could be statically initialized by the application	*/
unsigned __vcm_maxres;		/* max number of vsegs resident at	*/
				/* any one time.			*/
				/*	0:	as many as possible	*/
				/*	1:	only one at a time	*/
				/*	n:	not supported		*/

STATIC int vcm_fd;		/* file descriptor for EXE file		*/
STATIC unsigned vcm_reserve;	/* segment of reserve buffer, big	*/
				/* enough for largest vseg		*/
STATIC unsigned vcm_reserve_u;	/* number of vseg currently occupying	*/
				/* the reserve buffer, 0 if none	*/

/* Least Recently Used list	*/
#define	LRUhead 0		/* least recently used			*/

extern void vcm_isr();		/* vseg interrupt service routine	*/
extern unsigned char _ovlflag;	/* set to !=0 if vseg handler is installed */
extern unsigned _ovlvec[2];	/* previous vseg interrupt vector	*/

static int (far cdecl *vcm_prev_handler)(void);

void _STI_vcm ( void );
void _STD_vcm ( void );
STATIC unsigned near pascal LRU_get ( void );
STATIC void near pascal LRU_append ( unsigned u );
STATIC void near pascal LRU_remove ( unsigned u );
STATIC void near gate_init ( void );
STATIC void near cdecl vcm_stackwalk ( unsigned segment , unsigned vcmnum );
STATIC void near vcm_error ( void );
STATIC void near pascal vcm_msg ( char *msg );
STATIC void near pascal vcm_seek ( unsigned long offset );
STATIC void near pascal vcm_read ( void *ph , unsigned size );
STATIC char *near pascal vcm_readline ( void );
STATIC int near vcm_filename ( void );
STATIC int far cdecl vcm_freememory ( void );
unsigned vcm_callgate ( unsigned onum );
STATIC void near vcm_loadfromdisk ( Vcm *v , unsigned vcmseg );
#ifdef ZDB
void	vcm_zdbload(unsigned vcmnum);
void	vcm_zdbunload(unsigned vcmnum);
void	vcm_zdbrefer(unsigned vcmnum);
#endif

/*******************************
 * Initialize system. Should be called before main() is called.
 * Using a name starting with "_STI" causes this function to be put
 * into the static constructor table. (!)
 */

void _STI_vcm()
{
    if (!_ovlflag)
    {	char *buffer;

	vcm_fd = vcm_filename();	/* find EXE file	*/
	gate_init();

	/* Allocate minimum vseg space, to guarantee that we will	*/
	/* always be able to load an vseg.				*/
	/* Allow for rounding up to next seg.				*/
	buffer = malloc(__vcmmaxsize + 16);
	if (!buffer)
	    vcm_error();		/* insufficient memory		*/
	vcm_reserve = (unsigned)(((unsigned long) buffer) >> 16) + 1;

	/* hook vseg interrupt	*/
	int_getvector(_intno,&_ovlvec[0],&_ovlvec[1]);
	int_setvector(_intno,FP_OFF(vcm_isr),FP_SEG(vcm_isr));

	vcm_prev_handler = _malloc_handler;	/* remember old one	*/
	_malloc_handler = vcm_freememory;

	_ovlflag++;
    }
}

/****************************
 * Terminate vseg system.
 * Using a name starting with "_STD" causes this function to be put
 * into the static destructor table. (!)
 */

void _STD_vcm()
{
    /* Wrapper so it can be called multiple times	*/
    if (_ovlflag)
    {
	/* Restore previous malloc handler	*/
	_malloc_handler = vcm_prev_handler;

	/* unhook vseg interrupt */
	int_setvector(_intno,_ovlvec[0],_ovlvec[1]);

	close(vcm_fd);			/* close vseg file		*/

	_ovlflag = 0;
    }
}

/*******************************
 * Functions to maintain list of in-memory vsegs, organized as a
 * least-recently-used list.
 */

/*******************************
 * Get least-recently-used in-memory vseg that is not fixed in memory.
 * Returns:
 *	NULL if none found
 */

STATIC unsigned near pascal LRU_get()
{
    if (vcmtbl[LRUhead].next == LRUhead)
	return 0;			/* list is empty		*/
    return vcmtbl[LRUhead].next;
}

/*************************
 * Append u to LRU list.
 */

STATIC void near pascal LRU_append(unsigned u)
{
	vcmtbl[u].next = LRUhead;
	vcmtbl[u].prev = vcmtbl[LRUhead].prev;
	vcmtbl[LRUhead].prev = u;
	vcmtbl[vcmtbl[u].prev].next = u;
}

/**************************
 * Remove u from LRU list.
 */

STATIC void near pascal LRU_remove(unsigned u)
{
    vcmtbl[vcmtbl[u].prev].next = vcmtbl[u].next;
    vcmtbl[vcmtbl[u].next].prev = vcmtbl[u].prev;
}

/*******************************
 * Initialize list of free gates.
 */

STATIC void near gate_init()
{   unsigned number;
    unsigned u;

    /* Set number of gates to be the same as the amount of nesting	*/
    /* that can appear on the stack. Each far call on the stack uses	*/
    /* a minimum of 6 bytes (4 for address + 2 for BP).			*/
    number = _chkstack() / 6;
    gates = (Gate *) malloc(sizeof(Gate) * number);
    if (!gates)
	vcm_error();

    /* Thread all the unused gates together	*/
    for (u = 0; u < number; u++)
    {
	gates[u].f.type = GATE_FREE;
	gates[u].f.next = gate_freelist;
	gate_freelist = &gates[u];
    }
#ifdef ZDB
	vcm_zdbload(FP_SEG(gates));			 /*  tell debugger where gates are  */
#endif
}

/*******************************
 * Calculate approximately how much memory is used up by vsegs that are
 * in memory.
 */

unsigned long vcm_memavail()
{   unsigned u;
    unsigned long avail = 0;

    for (u = 1; u <= __vcmmax; u++)
    {
	Vcm *v = &vcmtbl[u];
	if (v->location == VCM_MEMORY && u != vcm_reserve_u)
	    avail += v->size;
    }
    return avail;
}

/*******************************
 * Walk the stack, replacing far returns to segment with a far return
 * to a gate which loads vseg vcmnum.
 */

STATIC void near cdecl vcm_stackwalk(unsigned segment, unsigned vcmnum)
{
    unsigned __ss *bp;
    unsigned bpn;

    /* A horrible hack to get the caller's BP	*/
    bp = (unsigned __ss *)(&segment)[-2];

    while (bp)
    {
	bpn = *bp;
	if (bpn & 1)		/* if far call on stack	*/
	{
	    /* bp[0] = previous BP + 1			*/
	    /* bp[1] = offset of return address		*/
	    /* bp[2] = segment of return address	*/

	    if (bp[2] == segment)
	    {	Gate *t;

		/* Allocate a gate for us	*/
		t = gate_freelist;
		assert(t);
		gate_freelist = t->f.next;

		/* Set the gate to be a call gate	*/
		t->cg.opcode = GATE_INT;
		t->cg.intnum = _intno;
		t->cg.vcmnum = vcmnum;
		t->cg.offset = bp[1];

		/* Poke the return address so it calls the gate	*/
		*(Gate far **)&bp[1] = t;
	    }

	    bpn--;
	}
	bp = (unsigned __ss *) bpn;
    }
}

/*******************************
 * A fatal non-recoverable error has occurred.
 * Write a message and abort.
 */

STATIC void near vcm_error()
{
    vcm_msg("Fatal error in vseg manager\r\n");
    abort();
}

/***********************************
 * Write out a message.
 */

STATIC void near pascal vcm_msg(char *msg)
{
    write(1,msg,strlen(msg));
}


/***********************************
 * Seek to a position in the EXE file.
 */

STATIC void near pascal vcm_seek(unsigned long offset)
{
    if (lseek(vcm_fd,offset,SEEK_SET) == -1L)
	vcm_error();
}

/***********************************
 * Read EXE file.
 */

STATIC void near pascal vcm_read(void *ph, unsigned size)
{
    if (read(vcm_fd,ph,size) != size)
	vcm_error();
}

/***********************************
 * Read line from user.
 * Return pointer to the ASCIIZ string.
 */

STATIC char * near pascal vcm_readline()
{   static unsigned char buffer[2 + 80] = { 80,0 };

    bdosx(0xA,buffer,0);
    buffer[buffer[1] + 2] = 0;	/* write over \r with a terminating 0	*/
    return (char *)(buffer + 2);
}

/***********************************
 * Find the vseg file name.
 * Open it for read and return the file descriptor for it.
 */

STATIC int near vcm_filename()
{   char *filename;
    char *p;
    int fd;
    extern unsigned aenvseg;

    if (_osmajor >= 3)			/* if DOS 3.0 or later		*/
    {
	p = MK_FP(aenvseg,0);
	do
	{   while (*p)
		p++;
	} while (*++p);
	p += 3;
    }
    else
    {
#if 0
	/* Search path for EXE file	*/
	char *s;
	extern char *$$EXENAM;

	p = $$EXENAM;
	s = getenv("PATH");
	if (!s)				/* if no PATH environment variable */
	    goto L1;
#else
	goto L2;			/* worry about DOS 2.0 later	*/
#endif
    }
    while ((fd = open(p,O_RDONLY)) == -1)
    {
    L1:
	vcm_msg("Can't open EXE file ");
	vcm_msg(p);
    L2:
	vcm_msg("\r\nEnter EXE file name:");
	p = vcm_readline();
    }
    return fd;
}

/***********************************
 * Called when memory is needed.
 * Returns:
 *	!=0	managed to find some memory to free
 *	0	failed to find any memory to free
 */

STATIC int far cdecl vcm_freememory()
{
    char *buffer;
    unsigned segment;
    Vcm *v;
    unsigned vcmnum;

    vcmnum = LRU_get();		/* get most recently used memory buffer	*/

    if (vcmnum)
    {
	/* Free buffer used by vseg v	*/
	v = &vcmtbl[vcmnum];
	segment = v->loc.umemory.segment;
	buffer = MK_FP(segment - 1,v->loc.umemory.offset);
	free(buffer);
	v->location = VCM_DISK;
#ifdef ZDB
	vcm_zdbunload(vcmnum);		   /*  tell debugger this overlay unloaded  */
#endif
	LRU_remove(vcmnum);

	vcm_stackwalk(segment,v - vcmtbl);

	return 1;
    }
    else
	/* We have no more memory to give. Perhaps the previous handler	*/
	/* has some.							*/
	return (*vcm_prev_handler)();
}

/*********************************
 * Load vseg.
 * Input:
 *	vnum		vseg number (1..__vcmmax)
 * Returns:
 *	segment of location where vseg is loaded
 */

unsigned vcm_callgate(unsigned vnum)
{   unsigned vcmseg;
    Vcm *v;
    char *buffer;

#if 0	/* Handled by OVLA.ASM */
    if (FP_SEG(g) == FP_SEG(gates))
    {
	/* The caller was a gate that is on the free list. Return	*/
	/* it to the free list						*/
	g->f.type = GATE_FREE;
	g->f.next = gate_freelist;
	gate_freelist = g;
    }
#endif

    v = &vcmtbl[vnum];
    switch (v->location)
    {
	case VCM_DISK:
	    /* malloc buffer for vseg, allowing for rounding up to next seg	*/
	    if (__vcm_maxres == 1)
		goto L1;
	    buffer = malloc(v->size + 16);
	    if (buffer)				/* if sufficient memory	*/
	    {	vcmseg = (unsigned)(((unsigned long) buffer) >> 16) + 1;
		LRU_append(vnum);	/* o is now most-recently-used */
	    }
	    else
	    {
	     L1:
		/* Use the reserve	*/
		if (vcm_reserve_u)		/* if already in use	*/
		{
		    vcm_stackwalk(vcm_reserve,vcm_reserve_u);
		    vcmtbl[vcm_reserve_u].location = VCM_DISK;	/* dump it out of memory */
#ifdef ZDB
			vcm_zdbunload(vcm_reserve_u);		 /*  tell ZDB its unloaded  */
#endif
		}
		vcmseg = vcm_reserve;		/* use reserve buffer	*/
		vcm_reserve_u = vnum;		/* mark as used by us	*/
	    }
	    vcm_loadfromdisk(v,vcmseg);

	    /* update *o data structure	*/
	    v->location = VCM_MEMORY;
	    v->loc.umemory.segment = vcmseg;
	    v->loc.umemory.offset = FP_OFF(buffer);
#ifdef ZDB
		vcm_zdbload(vnum);					  /*  tell debugger its loaded  */
#endif

	    break;
	case VCM_MEMORY:
	    vcmseg = v->loc.umemory.segment;
#ifdef ZDB
		vcm_zdbrefer(vnum);			 /*  tell debugger its been referenced  */
#endif

	    /* Move v to end of LRU list, since it's the MRU	*/
	    if (vcmseg != vcm_reserve)
	    {	LRU_remove(vnum);
		LRU_append(vnum);
	    }
	    break;
    }
    return vcmseg;
}

/*****************************
 * Load vseg from disk.
 * Input:
 *	v	vseg to load
 *	vcmseg	segment of where to put it
 */

STATIC void near vcm_loadfromdisk(Vcm *v,unsigned vcmseg)
{   struct header h;
    unsigned us;
    unsigned numreloc;

    /* read vseg from disk into buffer	*/
    vcm_seek(v->diskoffset);
    vcm_read(MK_FP(vcmseg,0),v->size);
    vcm_read(&numreloc,2);		/* number of relocation items	*/

    /* perform relocation relative to root	*/
    while (numreloc)			/* while more relocation items	*/
    {
#define CHUNK 256
	unsigned short reloc[CHUNK];
	unsigned short __ss *r;
	unsigned short far *p;

	us = (numreloc < CHUNK) ? numreloc : CHUNK;
	numreloc -= us;
	vcm_read(reloc,2 * us);
	for (r = reloc; us--; r++)
	{
	    p = MK_FP(vcmseg,r[0]);
	    *p += _psp + 0x10;	/* start of code (offset by size of PSP) */
	}
    }
}

#ifdef ZDB

/*
	debugger hooks

	called for every load and unload of a segment so ZDB can catch the
	movement of vsegs.

	Note that vcm_zdbload must be called AFTER the relevant vcm structures
	are filled in so ZDB can read the loaded segment etc.

	vcm_zdbrefer allows the debugger to single step across VCM calls
*/

void vcm_zdbload(unsigned vcmnum)
{
}

void vcm_zdbunload(unsigned vcmnum)
{
}

void vcm_zdbrefer(unsigned vcmnum)
{
}
#endif

#endif /* vseg model */
