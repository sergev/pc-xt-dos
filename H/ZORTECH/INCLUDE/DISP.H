/*_ disp.h   Thu Dec  6 1990 */
/* Copyright (C) 1985-1990 by Walter Bright	*/
/* All rights reserved					*/
/* Written by Walter Bright				*/

#ifndef __DISP_H
#define __DISP_H	1	/* prevent multiple #include's		*/

#if __cplusplus
extern "C" {
#endif

extern	int
	_cdecl disp_printf(char *,...),
	_cdecl disp_getmode(void),
	_cdecl disp_getattr(void),
	_cdecl disp_putc(int);
extern	void
        _cdecl disp_levelblockpoke(int,int,int,int,unsigned,unsigned *,unsigned,unsigned *,unsigned),
	_cdecl disp_open(void),
	_cdecl disp_puts(const char *),
	_cdecl disp_box(int,int,unsigned,unsigned,unsigned,unsigned),
	_cdecl disp_close(void),
	_cdecl disp_usebios(void),
	_cdecl disp_move(int,int),
	_cdecl disp_flush(void),
	_cdecl disp_eeol(void),
	_cdecl disp_eeop(void),
	_cdecl disp_startstand(void),
	_cdecl disp_endstand(void),
	_cdecl disp_setattr(int),
	_cdecl disp_setcursortype(int),
	_cdecl disp_pokew(int,int,unsigned short),
	_cdecl disp_scroll(int,unsigned,unsigned,unsigned,unsigned,unsigned),
	_cdecl disp_setmode(unsigned char),
	_cdecl disp_peekbox(unsigned short *,unsigned,unsigned,unsigned,unsigned),
	_cdecl disp_pokebox(unsigned short *,unsigned,unsigned,unsigned,unsigned),
	_cdecl disp_set43(void),
	_cdecl disp_reset43(void),
	_cdecl disp_fillbox(unsigned,unsigned,unsigned,unsigned,unsigned),
	_cdecl disp_hidecursor(void),
	_cdecl disp_showcursor(void);
extern	unsigned short _cdecl disp_peekw(int,int);

#if M_UNIX || M_XENIX
extern unsigned char _cdecl disp_scomode, _cdecl disp_captured;
extern int _cdecl disp_vt_number;
extern char _cdecl *disp_offset;
#endif

/***************************
 * Some attributes for the text display adapters for disp_setattr():
 */

#define DISP_REVERSEVIDEO	0x70
#define DISP_NORMAL		0x07
#define DISP_UNDERLINE		0x01
#define DISP_NONDISPLAY		0x00

/* The following attribute bits should be OR'ed in:	*/
#define DISP_INTENSITY		0x08
#define DISP_BLINK		0x80

/*****************************
 * Values for disp_setcursortype():
 */

#define DISP_CURSORBLOCK (disp_mono ? 0x000C : 0x0007)	/* block cursor	    */
#define DISP_CURSORHALF	 (disp_mono ? 0x060C : 0x0307)	/* half-size cursor */
#define DISP_CURSORUL	 (disp_mono ? 0x0B0C : 0x0707)	/* underline cursor */

/******************************
 * Display state.
 */

#pragma ZTC align 1

typedef struct disp_t
{
    unsigned short numrows;	/* # of rows in display			*/
    unsigned short numcols;	/* # of columns in display		*/
    unsigned short cursorrow;	/* cursor position			*/
    unsigned short cursorcol;
    unsigned char mono;		/* 0 for color, 1 for monochrome	*/
    unsigned char snowycga;	/* if 1, then the display is an IBM snowy CGA	*/
    unsigned char mode;		/* current display mode			*/
    unsigned char inited;	/* 1 if display package is opened	*/
    unsigned char ega;		/* 1 if IBM EGA				*/
    unsigned char reserved[3];	/* round out to dword size		*/

    union _disp
    {
	unsigned short _far *base;	/* base pointer into screen	*/
	struct _disp1
	{   unsigned offset;
	    unsigned short base;
	} _disp1x;
    } _dispx;
} disp_t;

#pragma ZTC align

extern disp_t _cdecl disp_state;

/* For backwards compatibility	*/
#define disp_numrows	disp_state.numrows
#define disp_numcols	disp_state.numcols
#define disp_cursorrow	disp_state.cursorrow
#define disp_cursorcol	disp_state.cursorcol
#define disp_mono	disp_state.mono
#define disp_snowycga	disp_state.snowycga
#define disp_mode	disp_state.mode
#define disp_inited	disp_state.inited
#define disp_ega	disp_state.ega
#define disp_base	disp_state._dispx._disp1x.base

#if __cplusplus
}
#endif

#endif /* __DISP_H */
