/*_ mou.c  Sat Jun  3 1989   Modified by: Dave Mansell	*/
/* Copyright (c) 1986-1989 by Walter Bright		*/
/* All Rights Reserved					*/
/* Written by Dave Mansell				*/
/* Modified by G. Eric Engstrom (GEE)			*/
/* Interface to the OS/2 MOU functions			*/

#ifdef __OS2__			/* Otherwise it's in msmouse.asm */
#define INCL_SUB
#define INCL_DOS

#ifdef __cplusplus
extern "C"
{
#endif
#include <os2.h>
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <msmouse.h>

#define IS_BN1 (MOUSE_BN1_DOWN | MOUSE_MOTION_WITH_BN1_DOWN)
#define IS_BN2 (MOUSE_BN2_DOWN | MOUSE_MOTION_WITH_BN2_DOWN)
#define IS_BN3 (MOUSE_BN3_DOWN | MOUSE_MOTION_WITH_BN3_DOWN)

typedef struct _MONBUF {
	USHORT length;
	UCHAR buffer[18];
} MONBUF;

typedef struct _MOUMONPACKET {
	USHORT fmonitor;
	MOUEVENTINFO MouData;
} MOUMONPacket;
typedef MOUMONPacket _far *PMOUMONPacket;

#define BUTTONLEFT	0
#define BUTTONMIDDLE	2
#define BUTTONRIGHT	1

static HMOU msm_inited;
static USHORT Thread1;
static int msm_reversebuttoncount = 0;

static struct MOUinfo_s
{
	UCHAR mode;
	USHORT state[3];
	USHORT press_c[3];
	USHORT release_c[3];
	USHORT pat;
	USHORT row;
	USHORT col;
	USHORT maxr;
	USHORT minr;
	USHORT maxc;
	USHORT minc;
} Ms;

static MOUEVENTINFO Mevent;
static VIOMODEINFO Vstate;

static unsigned pel_row, pel_col, Killthread;

#ifdef __cplusplus
extern "C"
  {
  static void _far msm_thread(void);
  }
#else
static void _far msm_thread(void);
#endif

static unsigned char Tstack[4096];
static int msm_cursorCount = 0;	/*gee, added cursor count for compatibility msmouse.asm */
USHORT monhand;

int msm_init(void)
{
	char *monname = "MOUSE$";

	if (MouOpen(NULL,&msm_inited))
		return (msm_inited = 0);
	else {
		Vstate.cb = sizeof(Vstate);
		VioGetMode(&Vstate,0);
		Ms.mode = (Vstate.fbType & VGMT_GRAPHICS) != 0;
		if (Ms.mode) {
			pel_row = Vstate.vres / Vstate.row -1;
			pel_col = Vstate.hres / Vstate.col -1;
		} else
			pel_row = pel_col = 8;
		Ms.maxr = Vstate.vres;
		Ms.maxc = Vstate.hres;
		Killthread = 0;
		DosMonOpen((unsigned char *) monname,&monhand);
		DosCreateThread(msm_thread,&Thread1,Tstack+sizeof(Tstack));
                msm_cursorCount = 0;	/*gee*/
		return -1;
	}
}

#ifdef __cplusplus
extern "C"
  {
#endif
static void _far msm_thread(void)
{
	int i, but[3];
	int flag;
	USHORT pktlen;
	PTRLOC t;
	char moninbuf[128], monoutbuf[128];
	MONBUF *inbuf, *outbuf;
	MOUMONPacket pkt;
	
	inbuf = (MONBUF *) moninbuf;
	outbuf = (MONBUF *) monoutbuf;
	inbuf->length = 128;
	outbuf->length = 128;

	Ms.state[BUTTONLEFT] = 0;
        Ms.state[BUTTONMIDDLE] = 0;
        Ms.state[BUTTONRIGHT] = 0;
	
	pktlen = sizeof(pkt);

	DosMonReg(monhand,(PBYTE) inbuf, (PBYTE) outbuf,MONITOR_BEGIN,-1);
	DosSetPrty(PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, Thread1);
	
	do {
		flag = 0;
		DosMonRead((PBYTE) inbuf, DCWW_WAIT, (PBYTE) &pkt, &pktlen);
		Ms.pat = pkt.MouData.fs;
		Ms.row = pkt.MouData.row*pel_row;
		Ms.col = pkt.MouData.col*pel_col;
                if (msm_reversebuttoncount > 0)
                  {
                  but[BUTTONRIGHT] = (Ms.pat & IS_BN1) != 0;
                  but[BUTTONLEFT]  = (Ms.pat & IS_BN2) != 0;
                  }
                else
                  {
                  but[BUTTONLEFT]  = (Ms.pat & IS_BN1) != 0;
                  but[BUTTONRIGHT] = (Ms.pat & IS_BN2) != 0;
                  }
                but[BUTTONMIDDLE] = (Ms.pat & IS_BN3) != 0;

		for (i=0; i < 3; i++) {
			if (but[i] != Ms.state[i])
				if ((Ms.state[i] = but[i]) != 0)
					++Ms.press_c[i];
				else
					++Ms.release_c[i];
		}
		if (Ms.row < Ms.minr || Ms.row > Ms.maxr) {
			Ms.row = (Ms.row < Ms.minr) ? Ms.minr : Ms.maxr;
			flag = 1;
		}
		if (Ms.col < Ms.minc || Ms.col > Ms.maxc) {
			Ms.col = (Ms.col < Ms.minc) ? Ms.minc : Ms.maxc;
			flag = 1;
		}
		if (flag) {
			t.row = Ms.row/pel_row;
			t.col = Ms.col/pel_col;
			MouSetPtrPos(&t,msm_inited);
		}
	} while (Killthread == 0);
        DosExit(EXIT_THREAD,0);
	return;
}
#ifdef __cplusplus
  }
#endif

void msm_term(void)
{
	if (msm_inited) {
		Killthread = 1;
		DosMonClose(monhand);
		MouClose(msm_inited);
		msm_inited = 0;
	}
}

void msm_showcursor(void)
{
	if (msm_inited
          && (++msm_cursorCount > 0))	/* gee */
		MouDrawPtr(msm_inited);
}

void msm_hidecursor(void)
{
	static NOPTRRECT area = { 0,0,24,79 };

	if (msm_inited
          && (--msm_cursorCount <= 0))	/* gee */
		if (Ms.mode) {
			area.cRow = Vstate.vres;
			area.cCol = Vstate.hres;
		} else {
			area.cRow = Vstate.row-1;
			area.cCol = Vstate.col-1;
		}
		MouRemovePtr(&area,msm_inited);
}

int msm_getstatus(unsigned *col,unsigned *row)
{
	int state = 0;
	PTRLOC pos;
	
	if (msm_inited) {
	        DosSleep(0);	/* GEE, remove info call, rely on mouMonitor to fill in all data.  */
		state = Ms.state[BUTTONLEFT]+
                        (Ms.state[BUTTONRIGHT]<<1)+
                        (Ms.state[BUTTONMIDDLE]<<2);	/* gee, made state compatible with msmouse.asm */
                *col = Ms.col;
                *row = Ms.row;
	}
	return (state);
}

void msm_reversebuttonon(void)
{
    ++msm_reversebuttoncount;
}

void msm_reversebuttonoff(void)
{
    --msm_reversebuttoncount;
}

int msm_reversebuttonis(void)
{
    return msm_reversebuttoncount;
}

int msm_reverebutton(int x)
{
    if (msm_reversebuttoncount > 0)
      {
      return (x - (x & 3)) | ((x & 2) >> 1) | ((x & 1) << 1);
      }
    else
      {
      return x;
      }
}

void msm_setcurpos(unsigned row, unsigned col)
{
	PTRLOC pos;

	pos.row = row;
	pos.col = col;
	
	if (msm_inited)
		if (MouSetPtrPos(&pos, msm_inited) != 0) {
			Mevent.row = row;
			Mevent.col = col;
		}
}


int msm_getpress(unsigned *count ,unsigned *col ,unsigned *row)
{
	int button = *count;
	
	if (msm_inited) {
        	DosSleep(0);	/* GEE */
		*count = Ms.press_c[button];
		Ms.press_c[button] = 0;
		*row = Ms.row;
		*col = Ms.col;
	}
	return (Ms.pat);
}
	
int msm_getrelease(unsigned *count ,unsigned *col ,unsigned *row)
{
	int button = *count;
	
	if (msm_inited) {
        	DosSleep(0);	/* GEE */
		*count = Ms.release_c[button];
		Ms.release_c[button] = 0;
		*row = Ms.row;
		*col = Ms.col;
	}
	return (Ms.pat);
}

void msm_setareax(unsigned minx,unsigned maxx)
{
	Ms.minc = minx;
	Ms.maxc = maxx;
}

void msm_setareay(unsigned miny,unsigned maxy)
{
	Ms.minr = miny;
	Ms.maxr = maxy;
}

void msm_setgraphcur(int a,int b,int *p){} /* not implemented */

void msm_settextcur(int a,int b,int c){} /* not useful under OS/2 */

void msm_signal(unsigned x,
	void (cdecl *y)(unsigned a,unsigned b,unsigned b,unsigned b),void *p){}
	/* not supported by OS/2 */

void msm_lightpenon(void){}		/* not supported by OS/2 */

void msm_lightpenoff(void){}		/* not supported by OS/2 */

void msm_setthreshhold(unsigned x) {}	/* not supported by OS/2 */

void msm_readcounters(int *mx,int *my)
{
	SCALEFACT ratio;

	static unsigned oldrow, oldcol;

	if (msm_inited) {
		MouGetScaleFact(&ratio,msm_inited);
                DosSleep(0);	/* GEE */
		*mx = (Mevent.row - oldrow)*ratio.rowScale*pel_row;
		*my = (Mevent.col - oldcol)*ratio.colScale*pel_col;
		oldrow = Mevent.row;
		oldcol = Mevent.col;
	}
	
}

void msm_setratio(unsigned ratiox, unsigned ratioy)
{
	SCALEFACT pos;
	
	pos.rowScale = ratioy;
	pos.colScale = ratiox;

	if (msm_inited)
		MouSetScaleFact(&pos,msm_inited);
}

void msm_condoff(unsigned uplx, unsigned uply ,unsigned lowrx, unsigned lowry)
{
	NOPTRRECT area = { 0,0,0,0 };

	if (msm_inited)
		{
		area.row = uply;
		area.col = uplx;
		area.cRow = lowry;
		area.cCol = lowrx;
		MouRemovePtr(&area,msm_inited);
	}
}

#endif

