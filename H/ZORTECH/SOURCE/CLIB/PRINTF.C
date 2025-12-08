/*_ printf.c   Thu Apr 13 1989   Modified by: Walter Bright */
/* $Header$ */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All Rights Reserved				*/
/* Written by Walter Bright			*/

#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<fltpnt.h>
#include	<float.h>
#include	"mt.h"

int _pformat(int (*)(int,void *),void *,const char *,va_list);

/***********************
 */

int fprintf(FILE *fp,const char *format,...)
{
	return vfprintf(fp,format,(va_list)&(format) + sizeof(format));
}

/***********************
 */

int vfprintf(FILE *fp,const char *format,va_list args)
{   int status;

    __fp_lock(fp);
    status = _pformat(_fputc_nlock,fp,format,args);
    __fp_unlock(fp);
}

/***********************
 */

int printf(const char *format,...)
{
	return vprintf(format,(va_list)&(format) + sizeof(format));
}

/***********************
 */

int vprintf(const char *format,va_list args)
{   int status;

    __fp_lock(stdout);
    status = _pformat(_fputc_nlock,stdout,format,args);
    __fp_unlock(stdout);
    if (stdout->_flag & _IOLBF)
	fflush(stdout);
    return status;
}

/*************************
 * Return number of chars written in array, excluding trailing 0.
 */

static _sprintf(int c,void *s)
{
	*(*(char **)s)++ = c;
	return c & 0xFF;
}

int sprintf(char *s,const char *format,...)
{
	return vsprintf(s,format,(va_list)&(format) + sizeof(format));
}

int vsprintf(char *s,const char *format,va_list args)
{	int i;
	char *sstart = s;

	if ((i = _pformat(_sprintf,&s,format,args)) < 0)
		i = 0;		/* an error occurred		*/
	sstart[i] = 0;
	return i;
}

/* Flags for flag word	*/
#define FLleft		1
#define	FLplus		2
#define	FLspc		4
#define FLhash		8
#define	FLlong		0x10
#define FLlngdbl	0x20
#define FL0pad		0x40
#define FLprec		0x80
#define FLuc		0x100
#define FLuns		0x200
#define FLlptr		0x400		/* far pointer format (with :)	*/
#define FLshort		0x800		/* 'h' modifier			*/
#define FLgformat	0x1000		/* g floating point format	*/

#define BUFMAX	32	/* temporary buffer size for _pformat()		*/

static long _near _pascal getlong(va_list __ss *,int);
static char _near * _near _pascal dosign(int,int);

/* Interface to ecvt functions */

#define ECVT	0
#define FCVT	1

char * _pascal
    __floatcvt(int cnvflag,char *digstr,double val,int ndig,int *pdecpt,int *psign);

void _pascal __doexponent(char __ss * __ss *ps,int exp);
char __ss * _pascal __trim0(char __ss *s);

/**********************************
 * Print formatting program.
 * Input:
 *	func	pointer to function to send the chars to. Returns char
 *		if successful, -1 if not.
 * Returns:
 *	# of chars sent to func else -1 if error
 */

int _pformat(int (*func)(int,void *),void *fp,const char *format,va_list pargs)
{	int nout;		/* # of chars sent to func()		*/
	int i,c;
	long l;
	unsigned long ul;
#if __I86__ >= 3
	unsigned long uls;
#else
#define	uls	0
#endif
	int pl;			/* length of prefix string		*/
	int width;		/* field width				*/
	int padding;		/* # of chars to pad (on left or right)	*/
	int precision;
	char buf[BUFMAX];
	char digstr[DBL_DIG*2 + 2];
	int sign;		/* sign for floating stuff		*/
	double dval;
	char _near *prefix;
	char *p;
	char *s;
	int sl;			/* length of formatted string s		*/
	int flags;		/* FLxxx				*/
	int base;		/* number base (decimal, octal, etc.)	*/
	int decpt;		/* exponent (base 10) for floats	*/
	int fpd;		/* classification of double value	*/

	nout = 0;
	while ((c = *format++) != 0)	/* while not end of format string */
	{	if (c != '%')	/* not a format control character	*/
		{	if ((*func)(c & 0xFF,fp) == EOF)
				goto err;
			nout++;
			continue;
		}

		prefix = (char _near *)"";	/* assume no prefix	*/
		flags = 0;			/* reset		*/
		while (1)
		{	c = *format++;
			switch (c)
			{
			    case '-':
				flags |= FLleft; /* left-justify	*/
				break;
			    case '+':
				flags |= FLplus; /* do + or - sign	*/
				break;
			    case ' ':
				flags |= FLspc;	/* space flag		*/
				break;
			    case '#':
				flags |= FLhash; /* alternate form	*/
				break;
			    case '0':
				flags |= FL0pad; /* pad with 0s		*/
				break;
			    case '*':
				width = va_arg(pargs,int);
				if (width < 0)	/* negative field width	*/
				{   flags |= FLleft;
				    width = -width;
				}
				c = *format++;
				goto getprecision;
			    case 0:		/* end of format string	*/
				goto err;
			    default:
				goto getwidth;
			}
		}

	    getwidth:
		width = 0;
		while (isdigit(c))
		{	width = width * 10 + c - '0';
			c = *format++;
		}

	    getprecision:
		precision = 0;
		if (c == '.')		/* if precision follows		*/
		{	flags |= FLprec;
			c = *format++;
			if (c == '*')
			{	precision = va_arg(pargs,int);
				if (precision < 0)
				{	flags &= ~FLprec;
					precision = 0;
				}
				c = *format++;
			}
			else
			{	/*if (c == '0')
				    flags |= FL0pad;*/ /* pad with 0s	*/
				while (isdigit(c))
				{	precision = precision * 10 + c - '0';
					c = *format++;
				}
			}
		}

		switch (c)
		{   case 'l':
			flags |= FLlong;
			c = *format++;
			break;
		    case 'h':
			flags |= FLshort;
			c = *format++;
			break;
		    case 'L':
			flags |= FLlngdbl;
			c = *format++;
			break;
		}

#if __I86__ >= 3
		uls = 0;
#endif
		switch (c)
		{
		    case 's':
			s = va_arg(pargs,char *);
			if (s == NULL)
			    s = "(null)";

			sl = strlen(s);		/* length of string	*/
			if (flags & FLprec)	/* if there is a precision */
			{	if (precision < sl)
					sl = precision;
					if (sl < 0)
						sl = 0;
			}
			break;

		    case '%':
			buf[0] = '%';
			goto L1;

		    case 'c':
			buf[0] = va_arg(pargs,int);
		    L1:
			s = &buf[0];
			sl = 1;
			break;

		    case 'd':
		    case 'i':
			base = 10;
			ul = l = getlong(&pargs,flags);
			if (l < 0)
			{	sign = 1;
				ul = -l;
			}
			else
				sign = 0;
			prefix = dosign(sign,flags);
			goto ulfmt;

		    case 'b':
			base = 2;
			goto getuns;

		    case 'o':
			base = 8;
			if (flags & FLhash)
			    prefix = (char _near *) "0";
			goto getuns;

		    case 'u':
			base = 10;
		    getuns:
			ul = getlong(&pargs,flags | FLuns);
			goto ulfmt;

		    case 'p':
#if sizeof(void *) > sizeof(int)	/* if far pointers		*/
			flags |= FLlong | FLlptr | FL0pad;
			if (!(flags & FLprec))
			    precision = 4 + 1 + sizeof(int) * 2;
#else
			flags |= FL0pad;
			if (flags & FLlong)
			    flags |= FLlptr;
			if (!(flags & FLprec))
			{   precision = sizeof(int) * 2;
			    if (flags & FLlptr)
				precision += 4 + 1;	/* seg:offset	*/
			}
#endif
		    case 'X':
			flags |= FLuc;
		    case 'x':
			base = 16;
			ul = getlong(&pargs,flags | FLuns);
#if __I86__ >= 3
			if (flags & FLlptr)		/* if far pointer */
			    /* Pull 16 bit segment off of arguments	*/
			    uls = getlong(&pargs,flags) & 0xFFFF;
#endif
			if ((flags & FLhash) && (ul | uls))
			    prefix = (char _near *)
				((flags & FLuc) ? "0X" : "0x");
			/* FALL-THROUGH */

		    ulfmt:
			{
			char __ss *sbuf;

			sbuf = &buf[BUFMAX - 1];
			if (ul | uls)
			{   do
			    {	char c;

				if (flags & FLlptr && sbuf == &buf[BUFMAX-1-(sizeof(int)*2)])
				{   c = ':';
#if __I86__ >= 3
				    ul = uls;		/* now work on seg */
				    uls = 0;
#endif
				}
				else
				{
				    c = (ul % base) + '0';
				    if (c > '9')
					c += (flags & FLuc) ? 'A'-'0'-10
							    : 'a'-'0'-10;
				    ul /= base;
				}
				*sbuf = c;
				sbuf--;
			    } while (ul | uls);
			    sbuf++;
			}
			else
			{   /* 0 and 0 precision yields 0 digits	*/
			    if (precision == 0 && flags & FLprec)
				sbuf++;
			    else
				*sbuf = '0';
			}
			sl = &buf[BUFMAX] - sbuf;
			if (precision > BUFMAX)
			    precision = BUFMAX;
			if (sl < precision)
			{
			    for (i = precision - sl; i--;)
			    {	--sbuf;
				*sbuf = (flags & FLlptr && sbuf == &buf[BUFMAX-1-(sizeof(int)*2)])
				    ? ':' : '0';
			    }
			    sl = precision;
			}
			s = (char *) sbuf;
			}
			break;

		    case 'f':
		    case 'F':
			if (!(flags & FLprec))	/* if no precision	*/
				precision = 6;	/* default precision	*/
			dval = va_arg(pargs,double);
			fpd = fpclassify(dval);
			if (fpd <= FP_INFINITE)	/* if nan or infinity	*/
			    goto nonfinite;
		    fformat:
			p = __floatcvt(FCVT,digstr,dval,precision,&decpt,&sign);
			prefix = dosign(sign,flags);
			{
			char __ss *sbuf;

			sbuf = &buf[0];
			if (decpt <= 0)
				*sbuf++ = '0';	/* 1 digit before dec point */
			while (decpt > 0 && sbuf < &buf[BUFMAX - 1])
			{	*sbuf++ = *p++;
				decpt--;
			}
			if (precision > 0 || flags & FLhash)
			{	int n;

				*sbuf++ = '.';
				while (decpt < 0 && precision > 0 &&
					sbuf < &buf[BUFMAX])
				{	*sbuf++ = '0';
					decpt++;
					precision--;
				}

				n = &buf[BUFMAX] - sbuf;
				if (n > precision)
				    n = precision;
				memcpy(sbuf,p,n);
				sbuf += n;

				/* remove trailing 0s	*/
				if ((flags & (FLgformat | FLhash)) == FLgformat)
				    sbuf = __trim0(sbuf);
			}
			sl = sbuf - &buf[0];	/* length of string	*/
			}
			s = &buf[0];
			break;
		    case 'e':
		    case 'E':
			if (!(flags & FLprec))	/* if no precision	*/
				precision = 6;	/* default precision	*/
			dval = va_arg(pargs,double);
			fpd = fpclassify(dval);
			if (fpd <= FP_INFINITE)	/* if nan or infinity	*/
			    goto nonfinite;
			p = __floatcvt(ECVT,digstr,dval,precision + 1,&decpt,&sign);
		    eformat:
			prefix = dosign(sign,flags);
			{
			char __ss *sbuf;

			buf[0] = *p;
			sbuf = &buf[1];
			if (precision > 0 || flags & FLhash)
			{	int n;

				buf[1] = '.';
				n = BUFMAX - 5 - 2;
				if (n > precision)
				    n = precision;
				sbuf = (char __ss *)memcpy(buf + 2,p + 1,n) + n;

				/* remove trailing 0s	*/
				if ((flags & (FLgformat | FLhash)) == FLgformat)
				    sbuf = __trim0(sbuf);
			}
			*sbuf++ = c;
			if (dval)		/* avoid 0.00e-01	*/
				decpt--;
			__doexponent(&sbuf,decpt);
			sl = sbuf - &buf[0];	/* length of string	*/
			}
			s = &buf[0];
			break;
		    case 'G':
		    case 'g':
			flags |= FLgformat;
			if (!(flags & FLprec))	/* if no precision	*/
				precision = 6;	/* default precision	*/
			dval = va_arg(pargs,double);
			fpd = fpclassify(dval);
			if (fpd <= FP_INFINITE)	/* if nan or infinity	*/
			{
			    static char fptab[][5] =
			    { "nans","nan","inf" };
			    static char fptablen[] = { 4,3,3 };

			nonfinite:
			    /* '#' and '0' flags have no effect	*/
			    flags &= ~(FL0pad | FLhash);
			    prefix = dosign(signbit(dval),flags);
#if 1 /* warty but small */
			    *(unsigned long *)buf = *(unsigned long *)fptab[fpd];
			    if (!(c & 0x20))	/* if c is upper case	*/
				/* Convert to upper case */
			    	*(unsigned long *)buf &= ~0x20202020;
#else /* portable */
			    strcpy(buf,fptab[fpd]);
			    if (isupper(c))
				strupr(buf);
#endif
			    sl = fptablen[fpd];	/* length of string	*/
			    s = &buf[0];
			    break;
			}
			p = __floatcvt(ECVT,digstr,dval,precision + 1,&decpt,&sign);
			/* decpt-1 is the exponent	*/
			if (decpt < -3 || decpt > precision)
			{			/* use e format		*/
			    c -= 'g' - 'e';
			    goto eformat;
			}
			else
			{   /* Convert precision to digits *after* dot	*/
			    precision -= decpt;	/* precision is >= 0	*/
			    goto fformat;
			}
		    case 'n':
			{	int *pi;

				/* Set *pi to # of chars so far */
				pi = va_arg(pargs,int *);
#if sizeof(int) == sizeof(long)
				if (flags & FLshort)
				    *(short *)pi = nout;
				else
				    *pi = nout;
#else
				if (flags & FLlong)
				    *(long *)pi = nout;
				else
				    *pi = nout;
#endif
			}
			continue;
		    default:
			goto err;
		}

		/* Send out the data. Consists of padding, prefix,	*/
		/* more padding, the string, and trailing padding	*/

		pl = strlen(prefix);		/* length of prefix string */
		nout += pl + sl;
		padding = width - (pl + sl);	/* # of chars to pad	*/

		/* if 0 padding, send out prefix (if any)	*/
		if (flags & FL0pad)
		    for (; *prefix; prefix++)
			if ((*func)(*prefix & 0xFF,fp) < 0) goto err;

		/* if right-justified and chars to pad			*/
		/*	send out prefix string				*/
		if (padding > 0)
		{   nout += padding;
		    if (!(flags & FLleft))
		    {	while (--padding >= 0)
			    if ((*func)((flags & FL0pad) ? '0'
							 : ' ',fp) < 0)
				 goto err;
		    }
		}

		/* send out prefix (if any)	*/
		for (; *prefix; prefix++)
			if ((*func)(*prefix & 0xFF,fp) < 0) goto err;

		/* send out string	*/
		for (i = 0; i < sl; i++)
			if ((*func)(s[i] & 0xFF,fp) < 0) goto err;

		/* send out right padding	*/
		if (flags & FLleft)
		{	while (--padding >= 0)
			    if ((*func)(' ',fp) < 0) goto err;
		}

	} /* while */
	return nout;

    err:
	return -1;
}

/***************************
 * Get an int or a long out of the varargs, and return it.
 */

static long _near _pascal getlong(va_list __ss *ppargs,int flags)
{	long l;

#if sizeof(int) == sizeof(long)
	l = va_arg(*ppargs,long);
	if (flags & FLshort)
	{	l = (short) l;
		if (flags & FLuns)
		    l &= 0xFFFFL;
	}
#else
	if (flags & FLlong)
		l = va_arg(*ppargs,long);
	else
	{	l = va_arg(*ppargs,int);
		if (flags & FLuns)
			l &= 0xFFFFL;
	}
#endif
	return l;
}

/***********************
 * Add sign to prefix.
 */

static char _near * _near _pascal dosign(int sign,int flags)
{
  return	(sign)		 ? (char _near *) "-" :
		(flags & FLplus) ? (char _near *) "+" :
		(flags & FLspc)	 ? (char _near *) " " :
				   (char _near *) "";
}
