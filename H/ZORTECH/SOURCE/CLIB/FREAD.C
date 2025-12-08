/*_ fread.c   Sun Apr 16 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1989 by Walter Bright	*/
/* All Rights Reserved					*/
/* Written by Walter Bright				*/

#include	<stdio.h>
#include	<string.h>
#include	<io.h>
#if MSDOS || __OS2__ || _WINDOWS
#include	<dos.h>
#endif
#include	"mt.h"

int _fillbuf(FILE *fp);
int _flushbu(int c,FILE *fp);
void _flushterm(void);

#if Afread

/***************************
 * Read data from stream.
 * Returns:
 *	# of complete items read
 */

size_t fread(void *pi,size_t bytesper,size_t numitems,FILE *fp)
{   char *p;
    unsigned bytesleft;
    unsigned u;

    p = (char *) pi;
    bytesleft = bytesper * numitems;
    __fp_lock(fp);
    if (fp->_flag & _IOTRAN)
    {	char *pend = p + bytesleft;

	while (p < pend)
	{   int c;

	    c = _fgetc_nlock(fp);
	    if (c != EOF)
	    {   *p = c;
		p++;
		continue;
	    }
	    numitems = (p - (char *) pi) / bytesper;
	    break;
	}
    }
    /* Deal with non-buffered io */
    else if (fp->_flag & _IONBF)
    {
	int cnt;

	/* The logic here mirrors _fillbuf()	*/

	if (fp->_flag & _IORW)		/* if read/write	*/
	    fp->_flag = (fp->_flag & ~_IOWRT) |  _IOREAD; /*set read bit */

	if ((fp->_flag & (_IOREAD | _IOERR | _IOEOF)) != _IOREAD)
	{   numitems = 0;		/* something is wrong here	*/
	    goto ret;
	}

	_flushterm();			/* flush terminal input output	*/
	cnt = read(fileno(fp),p,bytesleft);

	if (cnt <= 0)			/* if unsuccessful read	*/
	{
	    if (cnt == 0)		/* if end of file	*/
	    {
		fp->_flag |= _IOEOF;
		if (fp->_flag & _IORW)
		    fp->_flag &= ~_IOERR;
	    }
	    else
		fp->_flag |= _IOERR;

	    numitems = 0;
	    goto ret;
	}
	numitems = cnt / bytesper;
    }
    else
    {
	while (bytesleft != 0)
	{
	   L1:
		u = fp->_cnt;		/* # of characters left in buffer */
		if (u == 0)
		{	if (_fillbuf(fp) == EOF)
			{   numitems = (p - (char *) pi) / bytesper;
			    break;
			}
			goto L1;
		}
		if (u > bytesleft)
			u = bytesleft;
#ifdef BIGBUF
		if (fp->_flag & _IOBIGBUF)
			peek(fp->_seg,(unsigned) fp->_ptr,p,u);
		else
#endif
			memcpy(p,fp->_ptr,u);
		fp->_ptr += u;
		fp->_cnt -= u;
		p += u;
		bytesleft -= u;
	}
    }
ret:
    __fp_unlock(fp);
    return numitems;
}

#endif /* Afread */

#if Afwrite

/*************************
 * Write data to stream.
 * Returns:
 *	# of complete items written
 */

size_t fwrite(const void *pi,size_t bytesper,size_t numitems,FILE *fp)
{   unsigned bytesleft;
    unsigned u;
    const char *p = pi;

    bytesleft = bytesper * numitems;
    __fp_lock(fp);
    if (fp->_flag & _IOTRAN)
    {	const char *pend = p + bytesleft;

	while (p < pend)
	{
	    if (_fputc_nlock(*p,fp) != EOF)
	    {	p++;
		continue;
	    }
	    numitems = (p - (const char *) pi) / bytesper;
	    break;
	}
    }
    /* Deal with non-buffered io */
    else if (fp->_flag & _IONBF)
    {	/* The logic here mirrors _flushbu()	*/

	if (fp->_flag & _IORW)
	    fp->_flag = (fp->_flag & ~_IOREAD) | _IOWRT;

	if ((fp->_flag & (_IOWRT | _IOERR | _IOEOF)) != _IOWRT)
	{   numitems = 0;
	    goto ret;
	}

	if (bytesleft &&
	  (bytesleft != (u=write(fileno(fp),(void *) pi,bytesleft))))
	{
	    fp->_flag |= _IOERR;
	    numitems = u / bytesper;
	}
    }
    else
    {
	while (bytesleft != 0)
	{
		u = fp->_cnt;
		if (u == 0)
		{
			if (_flushbu(*p,fp) == EOF)
			{   numitems = (p - (const char *) pi) / bytesper;
			    break;
			}
			bytesleft--;
			p++;
			continue;
		}
		if (u > bytesleft)
			u = bytesleft;
#ifdef BIGBUF
		if (fp->_flag & _IOBIGBUF)
			poke(fp->_seg,(unsigned) fp->_ptr,p,u);
		else
#endif
			memcpy(fp->_ptr,p,u);
		fp->_ptr += u;
		fp->_cnt -= u;
		bytesleft -= u;
		p += u;
	}
    }
ret:
    __fp_unlock(fp);
    return numitems;
}

#endif /* Afwrite */

#if Aputs

/*****************************
 * Write string to stdout, followed by a newline.
 * Returns:
 *	0 if successful
 *	non-zero if not
 */

int puts(const char *p)
{	int result;

	__fp_lock(stdout);
	while (*p)
	{	if (putchar(*p) == EOF)
			return 1;
		p++;
	}
	result = putchar('\n') == EOF;
	__fp_unlock(stdout);
	return result;
}

#endif /* Aputs */

#if Agets

/****************************
 * Read string from stdin.
 */

char *gets(char *s)
{	unsigned c;
	char *sstart = s;

	__fp_lock(stdin);
	c = getchar();
	if (c == EOF)
	    goto err;
	while (c != EOF && c != '\n')
	{	*s++ = c;
		c = getchar();
	}
	*s++ = 0;			/* terminate the string		*/
	if (ferror(stdin))
err:
	    sstart = NULL;
	__fp_unlock(stdin);
	return sstart;
}

#endif /* Agets */

#if Afgets

/*************************
 * Read string from stream.
 * Returns:
 *	NULL no chars read or read error
 *	else s
 */

char *fgets(char *s,int n,FILE *fp)
{	unsigned c;
	char *sstart = s;

	__fp_lock(fp);
	while (--n > 0)
	{	c = _fgetc_nlock(fp);
		if (c != EOF)
		{
		    if ((*s = c) != '\n')
		    {	s++;
			continue;
		    }
		    s++;
		    break;
		}
		else
		{   if (s == sstart)	/* if no chars read		*/
			goto err;
		    else
			break;
		}
	}
	*s = 0;				/* terminate the string		*/
	if (ferror(fp))
err:
	    sstart = NULL;
ret:
	__fp_unlock(fp);
	return sstart;
}

#endif /* Afgets */

#if Afputs

/**************************
 * Write string to stream.
 * Returns:
 *	non-zero if error
 */

int fputs(const char *s,FILE *fp)
{	int result;

	__fp_lock(fp);
	while (*s)
	{	if (_fputc_nlock(*s,fp) == EOF)
		{   result = EOF;
		    goto ret;
		}
		s++;
	}
	result = 0;
ret:
	__fp_unlock(fp);
	return result;
}

#endif /* Afputs */
