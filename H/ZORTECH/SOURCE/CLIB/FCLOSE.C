/*_ fclose.c   Sat Sep	1 1990	 Modified by: Walter Bright */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All Rights Reserved				*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<io.h>
#include	<fcntl.h>
#include	<string.h>
#include	<process.h>
#include	<sys/stat.h>
#include	<errno.h>
#include	"mt.h"

#if MSDOS || __OS2__ || _WINDOWS
#include	<dos.h>
#endif

struct __FILE2
{	int tmpnum;
#ifdef _MT
	long semaphore;		/* for multithreading control	*/
	int nestcount;		/* so we can nest the locks	*/
	int threadid;		/* thread that has the lock	*/
#endif
};

extern struct __FILE2 _iob2[];
#define _tmpnum(fp)	_iob2[(fp) - _iob].tmpnum

FILE * _pascal __fopen(const char *name,const char *mode,FILE *fp);
void _pascal _freebuf(FILE *fp);

#if Afopen

/*************
 * Open file.
 */

FILE *fopen(const char *name,const char *mode)
{	FILE near *fp;
	FILE *fpr;

	/* Search for a free slot	*/
	for (fp = &_iob[0]; fp < &_iob[_NFILE]; fp++)
	{
		__fp_lock(fp);
		if (!(fp->_flag & (_IOREAD | _IOWRT | _IORW)))
		{
			fpr = (FILE *) __fopen(name,mode,fp);
			__fp_unlock(fp);
			return fpr;
		}
		__fp_unlock(fp);
	}

	/* failed to find one	*/
	return NULL;
}

#endif /* FOPEN */

#if Afreopen

/********************
 * Reopen existing channel.
 */

FILE *freopen(const char *name,const char *mode,FILE *fp)
{	FILE *fpr;

	__fp_lock(fp);
	if (fp->_flag & (_IOREAD | _IOWRT | _IORW))	/* if file is open */
		fclose(fp);				/* close it	*/
	fpr = __fopen(name,mode,fp);			/* reopen it	*/
	__fp_unlock(fp);
	return fpr;
}

#endif /* FREOPEN */

#if A_fopen

/*****************************
 * Open a file for input.
 * Input:
 *	name	pointer to drive, path and file name
 *	mode	one of the following strings:
 *		"r"	read
 *		"w"	write
 *		"a"	append to end of file
 *		"r+"	r/w, position at beginning of file
 *		"w+"	r/w, create or truncate file
 *		"a+"	r/w, position at end of file
 *		A 'b' may be appended to the string to indicate
 *		that the I/O is to be in binary (untranslated) mode.
 * Returns:
 *	NULL	if error
 *	fp	if successful
 */

FILE * _pascal __fopen(const char *name,const char *mode,FILE *fp)
{	int flag,fd;
	char append;

	if (!name || !fp || !*name)
		goto err;

	flag = _IOTRAN;			/* default is translated mode	*/
	append = 0;
	switch (*mode++)
	{	case 'r':
			flag |= _IOREAD;
			break;
		case 'a':
			append++;	/* = 1				*/
		case 'w':
			flag |= _IOWRT;
			break;
		default:
		err:
			return (FILE *) NULL;
	}

	/* Parse other flags, stopping on unrecognized ones	*/
	while (1)
	{   switch (*mode)
	    {
		case '+':
		    flag |= _IORW;
		    goto L3;
		case 'b':		/* binary mode			*/
		    flag &= ~_IOTRAN;	/* turn off default translated mode */
		L3: mode++;
		    continue;
	    }
	    break;
	}

	if (flag & _IORW)
	{   if (flag & _IOWRT && !append)	/* if "w+"		*/
		goto L2;			/* create file		*/
	    /* "r+" or "a+"	*/
	    if ((fd = open(name,O_RDWR)) == -1)
	    {	if (flag & _IOWRT)		/* if "a+"		*/
		    goto L2;			/* create file		*/
	    }
	    else if (append)
		goto L1;
	}
	else if (flag & _IOWRT)
	{   if (append)				/* if "a"		*/
	    {	if ((fd = open(name,O_WRONLY)) == -1)
		    goto L2;
		else
		L1: /* Position at end of file	*/
		    if (lseek(fd,0L,SEEK_END) == -1L)
		    {	close(fd);
			goto err;
		    }
	    }
	    else
	    {
#if M_UNIX | M_XENIX
	    L2:	fd = open(name,O_TRUNC | O_CREAT | O_RDWR,S_IWRITE | S_IREAD);
#else
	    L2:	fd = creat(name,S_IWRITE | S_IREAD);
#endif
	    }
	}
	else /* if "r"	*/
		fd = open(name,O_RDONLY);

	if ((fp->_file = fd) == -1)
		goto err;

	fp->_flag = (flag & _IORW) ? (flag & ~(_IOREAD | _IOWRT)) : flag;
#ifdef BIGBUF
	fp->_seg =
#endif
	fp->_cnt = 0;
	fp->_ptr = fp->_base = NULL;

	return fp;
}

#endif /* _FOPEN */

#if Aiob

#if M_UNIX || M_XENIX
char *_bufendtab[_NFILE];
FILE _iob[_NFILE] =
{
	{0,0,0,_IOREAD,0},		/* stdin	*/
	{0,0,0,_IOWRT ,1},		/* stdout	*/
	{0,0,0,_IOWRT | _IONBF,2},	/* stderr	*/
#else
FILE _iob[_NFILE] =
{
	{0,0,0,_IOTRAN | _IOREAD,0,1},		/* stdin	*/
	{0,0,0,_IOTRAN | _IOWRT ,1,1},		/* stdout	*/
	{0,0,0,_IOTRAN | _IOWRT | _IONBF ,2,1},	/* stderr	*/
	{0,0,0,_IOTRAN | _IORW	,3,1},		/* stdaux	*/
	{0,0,0,_IOTRAN | _IOWRT ,4,1},		/* stdprn	*/
#endif
	/* the rest have all 0 entries	*/
};

struct __FILE2 _iob2[_NFILE];

/****************************
 * Called by exit() to close all the open streams.
 */

#if !(__SMALL__ || __COMPACT__)	/* if large code model	*/
/* So that __fcloseall() is not at offset 0 in this segment	*/
static void __dummy() {}
#endif

void _near __fcloseall()
{   FILE _near *fp;

    for (fp = &_iob[0]; fp < &_iob[_NFILE]; fp++)
    {	__fp_lock(fp);
	if (fp->_flag & (_IOREAD | _IOWRT | _IORW))	/* if file is open */
	    fclose(fp);
	__fp_unlock(fp);
    }
}

void (_near *_fcloseallp)(void) = __fcloseall;

#endif /* IOB */

#if Afclose

/*********************
 * Close file stream.
 * Returns:
 *	0 if successful
 *	EOF if error
 */

int fclose(FILE *fp)
{   int result,flag;

    if (!fp)
	result = EOF;
    else
    {
	result = 0;
	__fp_lock(fp);
	flag = fp->_flag;

	/* if stream is open	*/
	if (flag & (_IOREAD | _IOWRT | _IORW))
	{	/* flush buffer	*/
		if (!(flag & _IONBF))
			result = fflush(fp);
		result |= close(fp->_file);
	}

#if 1
	{   char string[L_tmpnam];
	    int tmpnum = _tmpnum(fp);

	    if (tmpnum)
		unlink(itoa(tmpnum,string,10));	/* delete tmpfile */
	    _tmpnum(fp) = 0;
	}
#endif
	_freebuf(fp);			/* dump buffer if there is one	*/
	memset(fp,0,sizeof(FILE));	/* set all fields to 0		*/
	__fp_unlock(fp);
    }
    return result;
}

#endif /* FCLOSE */

#if Afflush

/***********************
 * Flush buffer.
 * Returns:
 *	0:	success
 *	EOF:	failure
 */

int fflush(FILE *fp)
{	int length;
	int result;

	/* don't flush buffer if we are not writing	*/
	__fp_lock(fp);
	if ((fp->_flag & (_IOWRT | _IONBF | _IOERR)) == _IOWRT &&
	    (fp->_base
#ifdef BIGBUF
		|| fp->_seg
#endif
			))
	{	length = fp->_ptr - fp->_base;	/* # of bytes in buffer	*/
#ifdef BIGBUF
		if (length && _writex(fp->_file,fp->_base,length,fp->_seg)
			!= length)
			fp->_flag |= _IOERR;
#else
		if (length && write(fp->_file,fp->_base,length)
			!= length)
			fp->_flag |= _IOERR;
#endif
		fp->_cnt = _bufsize(fp);
		fp->_ptr = fp->_base;
	}
	else
		fp->_cnt = 0;
	result = (ferror(fp)) ? EOF : 0;
	__fp_unlock(fp);
	return result;
}

#endif /* FFLUSH */

#if Aatexit

/********************************
 * Register function to be called by exit().
 * The functions are called in the reverse order that they are registered.
 * A maximum of 32 functions can be registered.
 * Returns:
 *	0	succeeds
 *	!=0	failed to register the function
 */

int atexit(void (*func)(void))
{
extern void  (* _near *_atexitp)(void);
extern void  (* _atexit_tbl[32+1])(void);

    if (!_atexitp)
	/* Position 0 is left NULL as a sentinal for the end		*/
	_atexitp = _atexit_tbl;
    if (_atexitp >= _atexit_tbl + 32)	/* if table is full		*/
	return 1;			/* fail				*/
    *++_atexitp = func;			/* register func		*/
    return 0;
}

#endif /* Aatexit */

#if Aexit

/**********************
 * Close all open files and exit.
 */

void (* _near *_atexitp)(void);
void (* _atexit_tbl[32+1])(void);	/* moved to exit, to provide support for exitstat.c */
void (* _hookexitp)(int errstatus);	/* provides support exitstat.c */
void (_near *_fcloseallp)(void);

void exit(int errstatus)
{
    if (_atexitp)			/* if any registered functions	*/
	while (*_atexitp)		/* while not end of array	*/
	{	(**_atexitp)();		/* call registered function	*/
		_atexitp--;
	}

    if (_hookexitp)			/* supports exitstat.h		*/
      {
      (*_hookexitp)(errstatus);		/* always calls exit_exit in exitstat.h */
      }
    else
    {
	_dodtors();			/* call static destructors	*/
	if (_fcloseallp)
	    (*_fcloseallp)();		/* close any streams		*/
	_exit(errstatus);
    }
}

#endif /* EXIT */

#if Aflushall

/************************
 * Flush all buffers, ignoring errors when flushing.
 * No files are closed.
 * Returns # of files that are open.
 */

int flushall()
{   FILE near *fp;
    int nopen;

    nopen = 0;
    for (fp = &_iob[0]; fp < &_iob[_NFILE]; fp++)
    {
	__fp_lock(fp);
	if (fp->_flag & (_IOREAD | _IOWRT | _IORW))
	{
	    fflush(fp);
	    nopen++;
	}
	__fp_unlock(fp);
    }
    return nopen;
}

#endif /* FLUSHALL */

#if Afcloseal

/**************************
 * Close all open files.
 * Returns:
 *	# of files closed
 */

int fcloseall()
{	FILE near *fp;
	int nclosed = 0;

	for (fp = &_iob[5]; fp < &_iob[_NFILE]; fp++)
	{	__fp_lock(fp);
		if (fp->_flag & (_IOREAD | _IOWRT | _IORW))
		{	fclose(fp);
			nclosed++;
		}
		__fp_unlock(fp);
	}
	return nclosed;
}

#endif /* Afcloseal */

#if Afmacros

/* Function implementations of stdio macros	*/

#undef getchar
int getchar(void) { return getc(stdin); }

#undef putchar
int putchar(int c) { return putc(c,stdout); }

#undef putc
int putc(int c,FILE *fp) { return fputc(c,fp); }

#undef getc
int getc(FILE *fp) { return fgetc(fp); }

#undef ferror
int ferror(FILE *fp) { return fp->_flag & _IOERR; }

#undef feof
int feof(FILE *fp) { return fp->_flag & _IOEOF; }

#undef clearerr
void clearerr(FILE *fp) { fp->_flag &= ~(_IOERR | _IOEOF); }

#undef rewind
void rewind(FILE *fp)
{
    __fp_lock(fp);
    fseek(fp,0L,SEEK_SET);
    fp->_flag&=~_IOERR;
    __fp_unlock(fp);
}

#endif /* Afmacros */

/****************************************************************/
/*  Function to buffer an already open'ed file.			*/
/*  Buffers an already open'ed file.				*/
/*								*/
/*  Parameters: fd   - Handle of open file to buffer		*/
/*		mode - Access mode of buffered I/O		*/
/*								*/
/*  Returns: FILE pointer to opened buffer if successful, else	*/
/*	     NULL.						*/
/*								*/
/*  Side effects: Temporarily opens then closes an additional	*/
/*		  buffered stream.				*/
/*								*/
/*  NOTE: No checking is done to assure that the stream mode	*/
/*	  is compatible with the rwmode used to open the	*/
/*	  original file.					*/
/* Written by Robert B. Stout					*/

#if Afdopen

FILE *fdopen(int fd, const char *mode)
{
	int fdup;
	FILE *fp;

	if (-1 == (fdup = dup(fd)))
		return NULL;
#if M_UNIX || M_XENIX
	if (NULL == (fp = fopen("/dev/null", mode)))
#else
	if (NULL == (fp = fopen("NUL", mode)))
#endif
	{
		close(fdup);
		return NULL;
	}
	__fp_lock(fp);
	close(fp->_file);
	fp->_file = fdup;
	__fp_unlock(fp);

	switch (*mode)
	{
	    case 'a':
		if (-1L == lseek(fdup, 0L, SEEK_END))
		    goto error;
		break;
	    case 'w':
		if (chsize(fdup, 0L))
		    goto error;
		break;
	    case 'r':
		if (-1L == lseek(fdup, 0L, SEEK_SET))
		    goto error;
		break;
	    default:
		goto error;
	}
	return fp;

error:
	fclose(fp);
	return NULL;
}

#endif

/***********************************
 * Set file translation mode to binary or translated mode.
 * (Note: This routine only works if the file handle has a corresponding
 *	  FILE* stream pointer)
 *	int setmode(int fd, int mode);
 * Input:
 *	fd	File handle (value returned by fileno())
 *	mode	Either O_TEXT for translated mode, O_BINARY for binary mode
 * Returns:
 *	-1 on error, errno is set to EINVAL or EBADF.
 *	otherwise, the previous translation mode is returned on success.
 * Graciously provided by E. McCreight.
 */

#if Asetmode

int setmode(int fd, int mode)
{   FILE *fp;
    int result;
    int flag;

    /* Search for the stream corresponding to fd	*/
    result = -1;
    for (fp = &_iob[0]; fp < &_iob[_NFILE]; fp++)
    {
	__fp_lock(fp);
	if (fileno(fp) == fd && fp->_flag & (_IOREAD | _IOWRT | _IORW))
	{
	    switch (mode)
	    {	case O_TEXT:	mode = _IOTRAN;	break;
		case O_BINARY:	mode = 0;	break;
		default:
		    errno = EINVAL;	/* invalid mode argument	*/
		    goto unlock;
	    }

	    flag = fp->_flag & _IOTRAN;
	    fp->_flag = (fp->_flag & ~_IOTRAN) | mode;
	    result = flag ? O_TEXT : O_BINARY;
unlock:	    __fp_unlock(fp);
	    goto ret;
	}
	__fp_unlock(fp);
    }
    errno = EBADF;			/* invalid file handle		*/
ret:
    return result;
}

#endif

#if Atmpnam

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

#endif

#if Afplock

/************************************
 * Routines to lock/unlock exclusive access to a stream.
 * These routines nest.
 * Be careful to use them exactly in a nesting manner.
 */

#undef __fp_lock
#undef __fp_unlock

void __fp_lock(FILE *fp)
{
#if _MT
    int result;
    struct __FILE2 _near *fp2;

    fp2 = &_iob2[fp - _iob];
    if (fp2->nestcount == 0 ||		/* if not already locked	*/
	*_threadid != fp2->threadid)	/* or a different thread	*/
    {	result = DOSSEMREQUEST(&fp2->semaphore,-1L);
	if (result != 0)
	    _semerr();
	fp2->threadid = *_threadid;
    }
    fp2->nestcount++;
#endif
}

void __fp_unlock(FILE *fp)
{
#if _MT
    int result;
    struct __FILE2 _near *fp2;

    fp2 = &_iob2[fp - _iob];
    if (--fp2->nestcount == 0)		/* if no more locks		*/
    {	result = DOSSEMCLEAR(&fp2->semaphore);
	if (result != 0)
	    _semerr();
    }
#endif
}

#endif
