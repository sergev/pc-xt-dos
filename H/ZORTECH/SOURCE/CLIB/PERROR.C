/*_ perror.c   Fri May 12 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1989 by Walter Bright	*/
/* All Rights Reserved					*/
/* Written by Walter Bright				*/
/* Modified for Windows 3.0 by G. Eric Engstrom		*/

#include	<stdio.h>
#include	<errno.h>
#include	<assert.h>
#include	<string.h>
#include	<stdlib.h>
#ifdef _WINDOWS
#include	"windows.h"
/*void _far _pascal FatalAppExit(unsigned short wAction,char _far *lpMessageText);*/
#endif

/**************************
 * Assertion failure.
 */

#ifdef Aassert

void _assert(const char *e,const char *file,unsigned line)
{
#ifdef _WINDOWS
unsigned   eLen;
unsigned   fLen;
    char   buf[81];
    char  *err;

    eLen = e ? strlen(e) : 0;
    fLen = file ? strlen(file) : 0;
    err = "line %u in file '%s' -> '%s'";
    if (eLen + fLen + 27 < 80)
      ;
    else
      err[(fLen + 27 < 80) ? 20 : 7] = 0;
    sprintf(buf,err,line,file,e); /* note: e and/or file will be ignored if there is not enough room */
    MessageBeep(0);
    MessageBox(NULL,buf,"ASSERTION FAILURE",MB_OK|MB_ICONSTOP|MB_TASKMODAL);
    FatalAppExit(0,"Warning: Memory may be corrupted.");
#else
    fprintf(stderr,"Assertion failure: '%s' on line %u in file '%s'\n",e,line,file);
    exit(EXIT_FAILURE);
#endif
}

#endif

/**************************
 * Print error message associated with the current
 * value of errno. MS-DOS has weird error numbers, and we add in
 * a few of our own, so the numbers are not sequential.
 */

#ifdef Aperror

void perror(const char *s)
{
#if 0
    fputs(s,stderr);
    fputs(": ",stderr);
    fputs(strerror(errno),stderr);
    fputc('\n',stderr);
#else
    fprintf(stderr,"%s: %s\n",s,strerror(errno));
#endif
}

/* Error messages indexed by error number	*/
char *sys_errlist[] =
{
	"no error",		/* 0		*/
	"invalid function",	/* 1		*/
	"no entity",		/* ENOENT	*/
	"not a directory",	/* ENOTDIR	*/
	"too many open files",	/* EMFILE	*/
	"access error",		/* EACCES	*/
	"bad file handle",	/* EBADF	*/
	"memory corrupted",	/* 7		*/
	"no memory",		/* ENOMEM	*/
		"invalid address",	/* 9		*/
};

/* Number of elements in sys_errlist[]	*/
int sys_nerr = sizeof(sys_errlist) / sizeof(sys_errlist[0]);

/*******************************
 * Map errnum into an error string.
 */

char *strerror(int errnum)
{
	static const struct ERR { char *msg; int no; } err[] =
	{
		"file exists",EEXIST,
		"too big",E2BIG,
		"can't execute",ENOEXEC,
		"domain error",EDOM,
		"range error",ERANGE,
		"no more files",18,
	};
	static char buf[8 + sizeof(errnum) * 3 + 1],*p;
	int i;

    if (errnum < sys_nerr)
	p = sys_errlist[errnum];
    else
    {   
	sprintf(buf,"errnum = %d",errnum);	/* default string	*/
	p = buf;
	for (i = 0; i < sizeof(err)/sizeof(struct ERR); i++)
		if (err[i].no == errnum)
		{	p = err[i].msg;
			break;
		}
    }
    return p;
}

#endif /* Aperror */
