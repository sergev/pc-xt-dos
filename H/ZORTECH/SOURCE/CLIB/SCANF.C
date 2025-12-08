/*_ scanf.c   Thu Apr 13 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All Rights Reserved				*/
/* Written by Walter Bright			*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static int near _sfmt(int (*)(int,void *),int (*)(void *),void *,const char *,va_list);

/**********************
 */

int fscanf(FILE *fp,const char *format,...)
{
	return _sfmt(ungetc,fgetc,fp,format,(va_list)&(format) + sizeof(format));
}

/*************************
 */

int scanf(const char *format,...)
{
	return _sfmt(ungetc,fgetc,stdin,format,(va_list)&(format) + sizeof(format));
}

/**************************
 * Input from a string.
 * Need a couple functions to make the string look like fgetc, ungetc.
 */

static int sgetc(void *ps)
{	return (*(*(char **)ps)) ? *(*(char **)ps)++ : EOF;
}

static int sungetc(int c,void *ps)
{	return (c == EOF) ? c : (*--(*(char **)ps) = c);
}

int sscanf(char *s,const char *format,...)
{
	return _sfmt(sungetc,sgetc,&s,format,(va_list)&(format) + sizeof(format));
}

/*************************
 * Input formatting routine.
 * Input:
 *	func -> function where we get the input from. -1 if no more input.
 *	uget -> unget function
 * Returns number of assigned input items, which can be 0.
 * Returns EOF if end of input before first conflict or conversion.
 */

#include <ctype.h>

#define FLneg	1
#define FLstar	2
#define	FLh	4
#define FLl	8
#define FLL	0x10

#define MAXINT	32767

#define SIGN	{switch (x) {	case '-': ss.flag |= FLneg; \
				case '+': x = GETCHAR; ss.width--; }}

#define GETCHAR		(ss.nread++, (*ss.func)(fp))
#define UNGET(c)	(ss.nread--, (*uget)(c,fp))

#define assign		(!(ss.flag & FLstar))

/****************************
 * Variables to save on parameter passing.
 * Collect all state variables into a struct.
 */

struct scanf_state
{
	int nread;	/* number of characters read from input		*/
	int nass;	/* number of assigned input items		*/
	int width;	/* field width					*/
	int flag;	/* mask of FLxxxx				*/
	int x;		/* last character read				*/
	char *p;	/* argument pointer				*/
	const char *format;	/* pointer to format string		*/
	void *fp;		/* argument to function pointers	*/
	int (*func)(void *);	/* where to get characters		*/
};

#define x ss.x
#define p ss.p
#define format ss.format
#define fp ss.fp

static int near pascal _scanfloat(struct scanf_state __ss *pss);
static int near pascal _scanre(struct scanf_state __ss *pss);

static int near _sfmt(int (*uget)(int,void *),int (*fgetc)(void *),void *fparg,
	const char *fmt,va_list pargs)
{
  int i;
  int base;
  long val;
  unsigned long ul;
  int gotone;
  struct scanf_state ss;
  char c;			/* last char read from format string	*/

  fp = fparg;
  format = fmt;			/* copy to a global			*/
  ss.func = fgetc;
  ss.nass = 0;
  ss.nread = 0;			/* # of chars read from func()		*/
  while ((c = *format++) != 0)	/* while not end of format string	*/
  {
	x = GETCHAR;		/* get next char of input		*/
    loop:
	if (x == EOF)
		goto eof;
	if (c == ' ' || c == '\t' || c == '\n')
	{
		do
			c = *format++;
		while (c == ' ' || c == '\t' || c == '\n');

		while (isspace(x))
			x = GETCHAR;
		if (c == 0)
			goto err;
		goto loop;
	}
	if (c == '%')
	{	ss.flag = 0;
		c = *format++;
		if (c == '*')		/* assignment suppression	*/
		{	ss.flag |= FLstar;
			c = *format++;
		}
		ss.width = 0;
		while (isdigit(c))
		{	ss.width = ss.width * 10 + c - '0';
			c = *format++;
		}
		if (ss.width == 0)
			ss.width = MAXINT;
		switch (c)
		{   case 'h':
			ss.flag |= FLh;
			goto L3;
		    case 'l':
			ss.flag |= FLl;
			goto L3;
		    case 'L':
			ss.flag |= FLL;
		    L3:	c = *format++;
			break;
		}
		if (c != '%' && assign)
			p = va_arg(pargs,char *);
		if (c != '[' && c != 'c' && c != 'n')
		{   while (isspace(x))
			x = GETCHAR;
		}

		switch (c)
		{
		    case 'i':
			SIGN;
			if (x != '0')
				goto decimal;
			x = GETCHAR;
			if (x == 'b' || x == 'B')
			{	base = 2;
				goto L2;
			}
			if (x == 'x' || x == 'X')
			{	base = 16;
			  L2:	x = GETCHAR;
				goto integer;
			}
			if (x <= '0' || x >= '9')
			{
			    UNGET(x);	/* restore non numeric character  */
			    x = '0';	/* treat single '0' as octal zero */
			}
			goto octal;		/* else octal		*/

		    case 'b':
			SIGN;
			base = 2;
			goto integer;

		    case 'o':
			SIGN;
		    octal:
			base = 8;
			goto integer;

		    case 'd':
			SIGN;
		    case 'u':
		    decimal:
			base = 10;
			goto integer;

		    case 'x':
		    case 'p':
		    case 'X':
			SIGN;
			base = 16;
			goto integer;

		    case 's':
			while (isspace(x))
			    x = GETCHAR;
			if (x == EOF)
				goto eof;
			if (assign)
				ss.nass++;
			while (ss.width-- && x != ' ' && x != '\n' && x != '\t')
			{	if (assign)
					*p++ = x;
				x = GETCHAR;
				if (x == EOF)
					break;
			}
			if (assign)
				*p = 0;		/* terminate the string	*/
			if (x == EOF)
			    goto eof;
			goto L1;

		    case 'c':
			if (ss.width == MAXINT)
				ss.width = 1;	/* read just 1 char	*/
			gotone = 0;
			while (ss.width--)
			{	if (x == EOF)
					goto done;
				gotone = 1;
				if (assign)
					*p++ = x;
				x = GETCHAR;
			}
			if (gotone)
			{   if (assign)
				ss.nass++;
			}
			else if (x == EOF)
			    goto eof;
		    L1:
			UNGET(x);
			continue;

		    case 'e':
		    case 'E':
		    case 'f':
		    case 'g':
		    case 'G':
		    case '[':
			if ((c == '[') ? _scanre(&ss) : _scanfloat(&ss))
				goto L1;
			else if (x == EOF)
				goto eof;
			else
				goto err;

		    case 'n':
			UNGET(x);
			if (assign)
			    *((int *) p) = ss.nread;
			continue;

		    case '%':
			goto match;
		    case 0:
			goto done;
		    default:
			goto err;
		}
	    integer:
		val = 0;
		gotone = 0;		/* don't have one just yet	*/
		while (ss.width--)
		{	if (!isxdigit(x))
				break;
			i = (x >= 'a') ? x - ('a' - 10) :
			    (x >= 'A') ? x - ('A' - 10) : x - '0';
			if (i < 0 || i >= base)
				break;
			else
			{	gotone = 1;	/* got at least one digit */
				val = val * base - i;
			}
			x = GETCHAR;
		} /* while */
		UNGET(x);
		if (gotone)
		{   if (assign)		/* if not suppressed	*/
		    {
			if (!(ss.flag & FLneg))
				val = -val;
			if (ss.flag & FLh)
				*((short *) p) = val;
			else if (ss.flag & FLl)
				*((long *) p) = val;
			else
				*((int *) p) = val;
			ss.nass++;
		    }
		}
		else if (x == EOF)
		    goto eof;
	}
	else
	{
	    match:			/* c must match input		*/
		if (x != c)
			goto err;
	}
  } /* while */

done:				/* end of format			*/
  return ss.nass;

err:
  (*uget)(x,fp);		/* push back offending input char	*/
  goto done;

eof:				/* end of file found on input		*/
  return (ss.nass) ? ss.nass : EOF;
}

#define ss (*pss)

/******************************
 * Do a floating point number.
 * Use atof() so we only have to get one conversion routine right. Just
 * load chars into a buffer and pass to atof().
 * Input:
 *	x = first char of number
 * Returns:
 *	x = char after the float, or EOF
 *	*p = double result
 *	non-zero: successful conversion
 *	0: failed conversion
 */

#define NBUFMAX 65

static int near pascal _scanfloat(struct scanf_state __ss *pss)
{   int dot,exp,digs;
    char nbuf[NBUFMAX + 1];	/* temp storage for float string	*/
    char __ss *f;
    static char snan[] = "ans";
    static char sinf[] = "nfinity";
    char _near *fs;
    int dblstate;
    char hex;			/* is this a hexadecimal-floating-constant? */

    dot = 0;			/* if decimal point has been seen */
    exp = 0;			/* if we're reading in the exponent */
    digs = 0;
    if (ss.width > NBUFMAX)
	    ss.width = NBUFMAX;	/* prevent overflow of nbuf[]	*/
    f = &nbuf[0];

    dblstate = 10;
    hex = 0;
    for (;; x = GETCHAR)
    {
	if (ss.width-- <= 0)
	    goto done;

	while (1)
	{   switch (dblstate)
	    {
		case 10:		/* initial state		*/
		    switch (x)
		    {	case '+':
			case '-':
			    dblstate = 0;
			    goto store;
		    }
		    /* FALL-THROUGH */
		case 0:			/* start of number		*/
		    switch (x)
		    {	case '0':
			    dblstate = 9;
			    digs = 1;	/* saw some digits		*/
			    break;
			case 'n':
			case 'N':	/* nan or nans			*/
			    fs = snan;
			    dblstate = 11;
			    break;
			case 'i':
			case 'I':	/* inf or infinity		*/
			    fs = sinf;
			    dblstate = 11;
			    break;
			default:
			    dblstate = 1;
			    continue;
		    }
		    break;

		case 11:		/* parsing strings		*/
		    if (isalpha(x) && tolower(x) == *fs)
			fs++;
		    else
		    {	/* Determine if we did a proper match	*/
			if (*fs == 0 || *fs == 's' ||
			    (fs - sinf == 2))
			    digs = 1;
			goto done;
		    }
		    break;

		case 9:
		    dblstate = 1;
		    if (x == 'X')
		    {	hex++;
			digs = 0;
			break;
		    }
		    /* FALL-THROUGH */

		case 1:			/* digits to left of .		*/
		case 3:			/* digits to right of .		*/
		case 7:			/* continuing exponent digits	*/
		    if (!isdigit(x) && !(hex && isxdigit(x)))
		    {   dblstate++;
			continue;
		    }
		    digs = 1;		/* saw some digits		*/
		    break;

		case 2:			/* no more digits to left of .	*/
		    if (x == '.')
		    {   dblstate++;
			break;
		    }
		    /* FALL-THROUGH */

		case 4:			/* no more digits to right of .	*/
		    if (x == 'E' || x == 'e' ||
			hex && (x == 'P' || x == 'p'))
		    {   dblstate = 5;
			hex = 0;	/* exponent is always decimal	*/
			break;
		    }
		    if (hex)
			goto error;	/* binary-exponent-part required */
		    goto done;

		case 5:			/* looking immediately to right of E */
		    dblstate++;
		    if (x == '-' || x == '+')
			break;
		    /* FALL-THROUGH */

		case 6:			/* 1st exponent digit expected	*/
		    if (!isdigit(x))
			goto error;	/* exponent expected		*/
		    dblstate++;
		    break;

		case 8:			/* past end of exponent digits	*/
		    goto done;
	    }
	    break;
	}
    store:
	*f++ = x;
    }
done:
    *f = 0;			/* terminate float string	*/
    if (digs)			/* if we actually saw some digits */
    {	if (assign)		/* if not suppressed		*/
	{   ss.nass++;
	    if (ss.flag & (FLl | FLL))
		*(double *)p = atof(nbuf);
	    else
		*(float *)p = atof(nbuf);
	}
	return 1;
    }
error:
    return 0;
}

/************************
 * Read in regular expression.
 * This is in a separate routine because of the large stack usage.
 * Input:
 *	x = first char of string
 * Returns:
 *	x = char after the string, or EOF
 *	*p = string
 *	non-zero: successful conversion
 *	0: failed conversion
 */

static int near pascal _scanre(struct scanf_state __ss *pss)
{	int j;
	int anymatch;
	char chartab[257];			/* +1 for EOF		*/
	char c;			/* last char read from format string	*/

#if __ZTC__
	/* Can't use _chkstack() if this routine is used in a DLL	*/
	/*_chkstack();*/		/* chartab[] uses a lot of stack */
#endif
	c = *format++;
	j = (c == '^') ? ((c = *format++),0) : 1; /* invert everything	*/
	chartab[EOF + 1] = 0;			/* no match for EOF	*/
	memset(chartab + 1,j ^ 1,256);
	do
	{	chartab[(unsigned char)c + 1] = j;
		c = *format++;
	} while (c != ']');
	anymatch = 0;
	while (ss.width-- && chartab[x + 1])
	{	if (assign)
		{	*p = x;
			*(p + 1) = 0;
			p++;
		}
		anymatch = 1;
		x = GETCHAR;
	}
	if (assign)
	    ss.nass += anymatch;
	return anymatch;	/* 1 if we matched any chars	*/
}

