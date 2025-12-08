/*_ strtod.c   Thu May 25 1989   Modified by: Walter Bright */
/* Copyright (C) 1985-1989 by Walter Bright	*/
/* All Rights Reserved					*/

#include	<stdlib.h>
#include	<ctype.h>
#include	<float.h>
#include	<string.h>
#include	<math.h>
#include	<fltpnt.h>

#if 0
static double negtab[] =
	{1e-256,1e-128,1e-64,1e-32,1e-16,1e-8,1e-4,1e-2,1e-1,1.0};

static double postab[] =
	{1e+256,1e+128,1e+64,1e+32,1e+16,1e+8,1e+4,1e+2,1e+1};
#else

/* Do things in explicit hex for accuracy	*/
static unsigned long hnegtab[][2] =
{
    { 0x64ac6f43, 0x0ac80628 },
    { 0xcf8c979d, 0x255bba08 },
    { 0x44f4a73d, 0x32a50ffd },
    { 0xd5a8a733, 0x3949f623 },
    { 0x97d889bc, 0x3c9cd2b2 },
    { 0xe2308c3a, 0x3e45798e },
    { 0xeb1c432d, 0x3f1a36e2 },
    { 0x47ae147b, 0x3f847ae1 },
    { 0x9999999a, 0x3fb99999 },
    { 0x00000000, 0x3ff00000 },
};

static unsigned long hpostab[][2] =
{
    { 0x7f73bf3c, 0x75154fdd },
    { 0xf9301d32, 0x5a827748 },
    { 0xe93ff9f5, 0x4d384f03 },
    { 0xb5056e17, 0x4693b8b5 },
    { 0x37e08000, 0x4341c379 },
    { 0x00000000, 0x4197d784 },
    { 0x00000000, 0x40c38800 },
    { 0x00000000, 0x40590000 },
    { 0x00000000, 0x40240000 },
};

#define negtab ((double *)hnegtab)
#define postab ((double *)hpostab)
#endif

#if __ZTC__ < 0x220

#undef NAN
static unsigned long nanarray[2] = {1,0x7FF80000 };
#define NAN	(*(double *)nanarray)

#undef NANS
static unsigned long nansarray[2] = {1,0x7FF00000 };
#define NANS	(*(double *)nansarray)

#undef INFINITY
static unsigned long infinityarray[2] = {0,0x7FF00000 };
#define INFINITY	(*(double *)infinityarray)

#endif

/*************************
 * Convert string to double.
 * Terminates on first unrecognized character.
 */

#if Astrtod

double strtod(const char *p,char **endp)
{
	double dval;
	int exp;
	unsigned long msdec,lsdec;
	unsigned long msscale;
	char hex,dot,sign,subject;
	int pow;
	const char *pinit = p;
	static char infinity[] = "infinity";
	static char nans[] = "nans";

	while (isspace(*p))
	    p++;
	sign = 0;			/* indicating +			*/
	switch (*p)
	{	case '-':
			sign++;
			/* FALL-THROUGH */
		case '+':
			p++;
	}
	subject = 0;
	dval = 0.0;
	dot = 0;			/* if decimal point has been seen */
	exp = 0;
	hex = 0;
	msdec = lsdec = 0;
	msscale = 1;

	switch (*p)
	{   case 'i':
	    case 'I':
		if (memicmp(p,infinity,8) == 0)
		{   p += 8;
		    goto L4;
		}
		if (memicmp(p,infinity,3) == 0)		/* is it "inf"?	*/
		{   p += 3;
		L4:
		    dval = HUGE_VAL;
		    subject = 1;
		    goto L3;
		}
		break;
	    case 'n':
	    case 'N':
		if (memicmp(p,nans,4) == 0)		/* "nans"?	*/
		{   p += 4;
		    dval = NANS;
		    goto L5;
		}
		if (memicmp(p,nans,3) == 0)		/* "nan"?	*/
		{   p += 3;
		    dval = NAN;
		L5:
		    if (*p == '(')		/* if (n-char-sequence)	*/
			goto L1;		/* invalid input	*/
		    subject = 1;
		    goto L3;
		}
	}

	if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
	{   p += 2;
	    hex++;
	    while (1)
	    {	int i = *p;

		while (isxdigit(i))
		{
		    subject = 1;	/* must have at least 1 digit	*/
		    i = isalpha(i) ? ((i & ~0x20) - ('A' - 10)) : i - '0';
		    if (msdec < (0xFFFFFFFF-16)/16)
			msdec = msdec * 16 + i;
		    else if (msscale < (0xFFFFFFFF-16)/16)
		    {	lsdec = lsdec * 16 + i;
			msscale *= 16;
		    }
		    else
			exp++;
		    exp -= dot;
		    i = *++p;
		}
		if (i == '.' && !dot)
		{	p++;
			dot++;
		}
		else
			break;
	    }
	    exp *= 4;
	    if (!subject)		/* if error (no digits seen)	*/
		goto L1;
	    if (*p == 'p' || *p == 'P')
		goto L2;
	    else
	    {	subject = 0;
		goto L1;		/* error, exponent is req'd	*/
	    }
	}
	else
	{
	    while (1)
	    {	int i = *p;

		while (isdigit(i))
		{
		    subject = 1;	/* must have at least 1 digit	*/
		    if (msdec < (0x7FFFFFFF-10)/10)
			msdec = msdec * 10 + (i - '0');
		    else if (msscale < (0x7FFFFFFF-10)/10)
		    {	lsdec = lsdec * 10 + (i - '0');
			msscale *= 10;
		    }
		    else
			exp++;
		    exp -= dot;
		    i = *++p;
		}
		if (i == '.' && !dot)
		{	p++;
			dot++;
		}
		else
			break;
	    }
	}
	if (!subject)			/* if error (no digits seen)	*/
	    goto L1;			/* return 0.0			*/
	if (*p == 'e' || *p == 'E')
	{
	    char sexp;
	    int e;
	L2:
	    sexp = 0;
	    switch (*++p)
	    {	case '-':
			sexp++;
			/* FALL-THROUGH */
		case '+':
			p++;
	    }
	    subject = 0;
	    e = 0;
	    while (isdigit(*p))
	    {
		if (e < DBL_MAX_EXP*2)		/* prevent integer overflow */
		    e = e * 10 + *p - '0';
		p++;
		subject = 1;
	    }
	    exp += (sexp) ? -e : e;
	    if (!subject)		/* if no digits in exponent	*/
		goto L1;		/* return 0.0			*/
	}

	dval = msdec;
	if (msscale != 1)		/* if stuff was accumulated in lsdec */
	    dval = dval * msscale + lsdec;

	if (dval)
	{   unsigned u;

	    if (hex)
	    {
		/* Exponent is power of 2, not power of 10	*/
		dval = ldexp(dval,exp);
		exp = 0;
	    }

	    u = 0;
	    pow = 256;
	    while (exp > 0)
	    {	while (exp >= pow)
		{	dval *= postab[u];
			exp -= pow;
		}
		pow >>= 1;
		u++;
	    }
	    while (exp < 0)
	    {	while (exp <= -pow)
		{	dval *= negtab[u];
			if (dval == 0)
				errno = ERANGE;
			exp += pow;
		}
		pow >>= 1;
		u++;
	    }
	    /* if overflow occurred		*/
	    if (dval == HUGE_VAL)
		errno = ERANGE;		/* range error			*/
	}

    L1:
	if (endp)
	{   if (subject == 0)		/* if subject string was empty	*/
		p = pinit;		/* indicate no conversion done	*/
	    *endp = (void *) p;
	}
    L3:
	return (sign) ? -dval : dval;
}

#endif

/***************************************
 * Do the same, but for floats.
 */

#if Astrtof

float strtof(const char *p,char **endp)
{   int errnosave = errno;
    double d;
    float f;

    errno = 0;
    d = strtod(p,endp);
    f = d;
    if (d != 0 && f == 0)		/* if underflow converting to float */
	errno = ERANGE;
    if (d != HUGE_VAL && f == HUGE_VAL)	/* if infinity converting to float */
	errno = ERANGE;
    if (errno != ERANGE)
	errno = errnosave;
    return d;
}

#endif

/*************************
 * Convert string to double.
 * Terminates on first unrecognized character.
 */

#if Aatof

double atof(const char *p)
{   int errnosave = errno;
    double d;

    d = strtod(p,(char **)NULL);
    errno = errnosave;
    return d;
}

#endif
