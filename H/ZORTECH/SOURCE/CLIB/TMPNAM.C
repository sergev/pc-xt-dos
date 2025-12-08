/*_ tmpnam.c   Thu Apr  5 1990   Modified by: Walter Bright */
/* Modified by Joe Huffman March 14, 1990 */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All rights reserved.					*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <errno.h>
#include <process.h>
#if MSDOS || __OS2__ || _WINDOWS
#include <dos.h>
#endif

struct __FILE2
{	int tmpnum;
#ifdef _MT
	long semaphore;		/* for multithreading control	*/
#endif
};

extern struct __FILE2 _iob2[];
#define _tmpnum(fp)	_iob2[(fp) - _iob].tmpnum

/*******************************
 * Generate temporary filename.
 * At least TMP_MAX different names are tried.
 * Input:
 *	string	Pointer to buffer to store filename into that
 *		is at least L_tmpnam bytes long. If NULL, then
 *		tmpnam() will use an internal static buffer which
 *		is overwritten by each call to tmpnam().
 * Returns:
 *	pointer to filename generated
 *	NULL if cannot generate a filename
 */

static unsigned __tmpnum;

char *tmpnam(char *string)
{   int save;
    static char buffer[L_tmpnam];
    static char seed;
    unsigned u;

    if (!string)
	string = buffer;

    if (!seed)			/* if first time through	*/
    {
	seed++;
#if MSDOS || __OS2__ || _WINDOWS
	__tmpnum = _psp;
#else
	__tmpnum = getpid();
#endif
    }

    save = errno;

    for (u = 0; u < TMP_MAX; u++)
    {
	/* Select a new filename to try	*/
	__tmpnum = (__tmpnum + 1) & 0x7FFF;
	if (__tmpnum == 0)
	    __tmpnum++;

	errno = 0;
	if (access(itoa(__tmpnum,string,10),F_OK) &&
	    errno != EACCES)
	    goto L1;
    }
    string = NULL;			/* failed miserably	*/
L1:
    errno = save;
    return string;
}

/***********************************
 * Generate temporary file.
 */

FILE *tmpfile(void)
{   FILE *fp;
    char name[L_tmpnam];

    fp = fopen(tmpnam(name),"wb+");
    if (fp)
    {
	_tmpnum(fp) = __tmpnum;
    }
    return fp;
}

/*******************************
 * Generate temporary filename.
 * Use the directory 'dir', use the prefix 'pfx' for the filename.
 * Input:
 *	dir	The subdirectory to put the file in.
 *	pfx	The prefix (up to five characters) for the file prefix.
 *	
 * Returns:
 *	pointer to the malloc'ed filename generated
 *	NULL if filename could not be generated
 */

char *tempnam (const char *dir, const char *pfx)
{   size_t i;
    char *ret_val;

    if (dir == NULL)
	dir = "";

    i = strlen(dir);
    ret_val = malloc(i+2 /* '\0' & '\\' */ + 8 /*root*/ + 4 /*.ext*/);
    if (ret_val)
    {
	int save;
	unsigned u;

	strcpy(ret_val,dir);

	/* Make sure directory ends with a separator	*/
#if MSDOS || __OS2__ || _WINDOWS
	if(i>0 && ret_val[i-1] != '\\' && ret_val[i-1] != '/' &&
	   ret_val[i-1] != ':')
	{
	    ret_val[i++] = '\\';
	}
#else
	if(i>0 && ret_val[i-1] != '/')
	    ret_val[i++] = '/';
#endif

	strncpy(ret_val + i, pfx, 5);
	ret_val[i + 5] = '\0';		/* strncpy doesn't put a 0 if more  */
	i = strlen(ret_val);		/* than 'n' chars.		    */
	save = errno;

	/* Prefix can have 5 chars, leaving 3 for numbers.
	   26 ** 3 == 17576
	 */
	for (u = 1; u < 26*26*26; u++)
	{
	    errno = 0;

	    itoa(u,ret_val + i,26);
	    strcat(ret_val,".tmp");
	    if (access(ret_val,F_OK) && errno != EACCES)
		goto L1;
	}
	free(ret_val);
	ret_val = NULL;			/* failed			*/
    L1:
	errno = save;
    }

    return ret_val;
}

