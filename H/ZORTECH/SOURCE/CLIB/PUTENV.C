/*_ putenv.c   Tue May 29 1990   Modified by: Walter Bright */
/* Copyright (C) 1990 by Walter Bright	*/
/* All Rights Reserved				*/
/* Written by Walter Bright			*/
/* DOS386 support added by G. Eric Engstrom     */

#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>

#if (__OS2__ && (MSDOS || _WINDOWS || DOS16RM || DOS386)) || (DOS16RM && DOS386)
#error	"invalid operating system #defines"
#endif

#if _WINDOWS
#include <dpmi.h>
#endif

#if DOS16RM
void _far * D16MemAlloc(unsigned);
int D16MemFree(void _far *);
#endif

#ifndef LPTR
#define LPTR	(sizeof(void *) > sizeof(int))
#endif

extern	char *_envptr;
#ifndef DOS386
extern	char *_envbuf;
extern	unsigned aenvseg;
#endif

#if _MT
#include "mt.h"
extern long __env_semaphore;
#endif

/****************************************
 * Returns:
 *	0	success
 *	-1	failure
 */

int putenv(const char *newstr)
{   const char *p;
    char deleting;
    char match;
    size_t len;
    size_t newlen;
    size_t namelen;
    size_t matchi,matchj;
    size_t newsize,oldsize;
    char *newbuf;
    char *newenvp;
    char *oldenvp;
    char *op;
    int result;

#if _MT
    if (DOSSEMREQUEST(&__env_semaphore,-1L))
	_semerr();
#endif

    if (!newstr)
	goto failure;

    /* Determine if we are deleting or modifying/adding	*/
    for (p = newstr; *p != '='; p++)
	if (!*p)			/* name followed by nothing	*/
	    goto failure;		/* is a failure			*/
    deleting = (p[1] == 0);		/* if ABC=			*/
    namelen = (p + 1) - newstr;		/* include the '=' in the length */
    newlen = (deleting) ? 0 : strlen(newstr) + 1;

    /* Get pointer to existing environment, oldenvp	*/
    oldenvp = _envptr;

    /* Determine new environment size, newsize		*/
    match = 0;
    newsize = 0;
    oldsize = 0;
    op = oldenvp;
    do
    {
	len = strlen(op) + 1;
	if (memcmp(op,newstr,namelen) == 0)	/* if match		*/
	{   if (match)
		goto failure;			/* duplicate def?	*/
	    matchi = oldsize;
	    matchj = oldsize + len;
	    match = 1;
	    newsize += newlen;
	}
	else
	    newsize += len;
	oldsize += len;
	op += len;
    } while (*op);
    if (!match)
    {	if (deleting)
	    goto success;
	/* We're adding a new environment variable	*/
	newsize += newlen;
	matchi = oldsize;
	matchj = oldsize;
    }
    oldsize++;
    newsize++;			/* allow for terminating 0 at end of env */

    /* Allocate new environment block, newenvp	*/
#if DOS16RM
    newenvp = (char *) D16MemAlloc(newsize);
    if (!newenvp)
	goto failure;
#elif DOS386
    newenvp = (char *) malloc(newsize + 15);
    if (!newenvp)
	goto failure;
#elif _WINDOWS
    newbuf = (char *) malloc(newsize + 15);
    if (!newbuf)
	goto failure;
#if LPTR
      {
      unsigned seg = FP_SEG(newbuf);
      unsigned off = FP_OFF(newbuf);

          off = (off + 15) & ~15;		/* round to next paragraph */
          if (_osmode == 1)	/* if running in protected mode windows */
            {
            short a = 0;
            char far *b;
            unsigned c;

            b = ((char *)dpmi_GetBaseAddress(seg)) + off;
            if ((a = dpmi_AllocLDTDescriptors(1)) != -1
              && dpmi_SetBaseAddress(a,b) != -1
              && dpmi_SetSegLimit(a,newsize) != -1)
              newenvp = (char *) MK_FP(a,0);
            else
              {
              free(newbuf);
              if (a != -1)
                dpmi_FreeDescriptor(a);
              newbuf = NULL;
              }
            }
          else
            newenvp = (char *) MK_FP(seg + (off >> 4),0);
      }
#else
      newenvp = (char *)(((unsigned short) newbuf + 15) & ~15);
#endif
#elif MSDOS
    newbuf = (char *) malloc(newsize + 15);
    if (!newbuf)
	goto failure;
#if LPTR
    {	unsigned seg = FP_SEG(newbuf);
	unsigned off = FP_OFF(newbuf);

	off = (off + 15) & ~15;		/* round to next paragraph */
	newenvp = (char *) MK_FP(seg + (off >> 4),0);
    }
#else
    newenvp = (char *)(((unsigned short) newbuf + 15) & ~15);
#endif
#elif __OS2__
    newenvp = (char *) malloc(newsize);
    if (!newenvp)
	goto failure;
#else
#error "OS undefined"
#endif

    /* Copy over new environment	*/
    memcpy(newenvp,oldenvp,matchi);
    memcpy(newenvp + matchi,newstr,newlen);
    memcpy(newenvp + matchi + newlen,oldenvp + matchj,oldsize - matchj);

    /* Dump old environment block	*/
#if DOS16RM
    if (_envbuf)
	D16MemFree(_envbuf);
    _envbuf = newenvp;
    _envptr = newenvp;
    aenvseg = FP_SEG(newenvp);
    *(unsigned _far *)MK_FP(_psp,0x2C) = aenvseg;
#elif DOS386
    free(_envptr);
    _envptr = newenvp;
#elif _WINDOWS
    if (_envbuf)
      {
      free(_envbuf);
      if (_osmode == 1)	/* if running in Protected mode */
	dpmi_FreeDescriptor(aenvseg);
      }
    _envbuf = newbuf;
    _envptr = newenvp;
#if LPTR
    aenvseg = FP_SEG(newenvp);
#else
    aenvseg = getDS() + ((unsigned) newenvp >> 4);
#endif
    *(unsigned _far *)MK_FP(_psp,0x2C) = aenvseg;
#elif MSDOS
    free(_envbuf);
    _envbuf = newbuf;
    _envptr = newenvp;
#if LPTR
    aenvseg = FP_SEG(newenvp);
#else
    aenvseg = getDS() + ((unsigned) newenvp >> 4);
#endif
    *(unsigned _far *)MK_FP(_psp,0x2C) = aenvseg;
#elif __OS2__
    if (FP_SEG(oldenvp) != aenvseg)	/* if not original		*/
	free(_envptr);
    _envptr = newenvp;
#else
#error "OS undefined"
#endif

success:
    result = 0;				/* success			*/

ret:
#if _MT
    if (DOSSEMCLEAR(&__env_semaphore))
	_semerr();
#endif

    return result;

failure:
    result = -1;
    goto ret;
}
