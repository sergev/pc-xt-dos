/*_ ecvt.c   Tue Feb 20 1990   Modified by: Walter Bright */
/* Copyright (C) 1986-1991 by Walter Bright	*/
/* All Rights Reserved				*/

#include	<stdlib.h>
#include	<string.h>
#include	<float.h>

#if _MT
#include	"mt.h"
#endif

/* Put this module into printf's code segment so we can use near calls on it */
#if __I86__ <= 2 && !(__SMALL__ || __COMPACT__)
#pragma ZTC cseg PRINTF_TEXT
#endif


#define DIGMAX	(DBL_DIG*2)	/* max # of digits in string		*/
				/* (*2 is a good fudge factor)		*/
#define DIGPREC	(DBL_DIG+2)	/* max # of significant digits		*/
				/* (+2 for fractional part after min	*/
				/* significant digits)			*/
#define ECVT	0
#define FCVT	1

#if !_MT
static char digstr[DIGMAX + 1 + 1];	/* +1 for end of string		*/
					/* +1 in case rounding adds	*/
					/* another digit		*/
#endif

#if 0
static double negtab[] =
	{1e-256,1e-128,1e-64,1e-32,1e-16,1e-8,1e-4,1e-2,1e-1,1.0};

static double postab[] =
	{1e+256,1e+128,1e+64,1e+32,1e+16,1e+8,1e+4,1e+2,1e+1};
#else

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

char * pascal
    __floatcvt(int cnvflag,char *digstr,double val,int ndig,int *pdecpt,int *psign);

/*************************
 * Convert double val to a string of
 * decimal digits.
 *	ndig = # of digits in resulting string
 * Returns:
 *	*pdecpt = position of decimal point from left of first digit
 *	*psign  = nonzero if value was negative
 */

char *ecvt(double val,int ndig,int *pdecpt,int *psign)
{
#if _MT
    return __floatcvt(ECVT,_getthreaddata()->t_digstr,val,ndig,pdecpt,psign);
#else
    return __floatcvt(ECVT,digstr,val,ndig,pdecpt,psign);
#endif
}


/*************************
 * Convert double val to a string of
 * decimal digits.
 *	nfrac = # of digits in resulting string past the decimal point
 * Returns:
 *	*pdecpt = position of decimal point from left of first digit
 *	*psign  = nonzero if value was negative
 */

char *fcvt(double val,int nfrac,int *pdecpt,int *psign)
{
#if _MT
    return __floatcvt(FCVT,_getthreaddata()->t_digstr,val,nfrac,pdecpt,psign);
#else
    return __floatcvt(FCVT,digstr,val,nfrac,pdecpt,psign);
#endif
}

/*************************
 * Convert double val to a string of
 * decimal digits.
 *	if (cnvflag == ECVT)
 *		ndig = # of digits in resulting string past the decimal point
 *	else
 *		ndig = # of digits in resulting string
 * Returns:
 *	*pdecpt = position of decimal point from left of first digit
 *	*psign  = nonzero if value was negative
 * BUGS:
 *	This routine will hang if it is passed a NAN or INFINITY.
 */

char * _pascal
    __floatcvt(int cnvflag,char *digstr,double val,int ndig,int *pdecpt,int *psign)
{	int decpt,pow,i;
	char c;

	*psign = (val < 0) ? ((val = -val),1) : 0;
	ndig = (ndig < 0) ? 0
			  : (ndig < DIGMAX) ? ndig
					    : DIGMAX;
	if (val == 0)
	{
		memset(digstr,'0',ndig);
		decpt = 0;
	}
	else
	{	/* Adjust things so that 1 <= val < 10	*/
		decpt = 1;
		pow = 256;
		i = 0;
		while (val < 1)
		{	while (val < negtab[i + 1])
			{	val *= postab[i];
				decpt -= pow;
			}
			pow >>= 1;
			i++;
		}
		pow = 256;
		i = 0;
		while (val >= 10)
		{	while (val >= postab[i])
			{	val *= negtab[i];
				decpt += pow;
			}
			pow >>= 1;
			i++;
		}

		if (cnvflag == FCVT)
		{	ndig += decpt;
			ndig = (ndig < 0) ? 0
					  : (ndig < DIGMAX) ? ndig
							    : DIGMAX;
		}

		/* Pick off digits 1 by 1 and stuff into digstr[]	*/
		/* Do 1 extra digit for rounding purposes		*/
		for (i = 0; i <= ndig; i++)
		{	int n;

			/* 'twould be silly to have zillions of digits	*/
			/* when only DIGPREC are significant		*/
			if (i >= DIGPREC)
				c = '0';
			else
			{	n = val;
				c = n + '0';
				val = (val - n) * 10;	/* get next digit */
			}
			digstr[i] = c;
		}
		if (c >= '5')		/* if we need to round		*/
		{	--i;
			while (1)
			{
				c = '0';
				if (i == 0)		/* if at start	*/
				{	ndig += cnvflag;
					decpt++;	/* shift dec pnt */
							/* "100000..."	*/
					break;
				}
				digstr[i] = '0';
				--i;
				c = digstr[i];
				if (c != '9')
					break;
			}
			digstr[i] = c + 1;
		} /* if */
	} /* else */
	*pdecpt = decpt;
	digstr[ndig] = 0;		/* terminate string		*/
	return digstr;
}


/**************************
 * Add exponent to string s in form +-nn.
 * At least 3 digits.
 */

void _pascal __doexponent(char __ss * __ss *ps,int exp)
{	register char __ss *s = *ps;

	*s++ = (exp < 0) ? ((exp = -exp),'-') : '+';
	*s++ = exp / 100 + '0';
	exp %= 100;
	*s++ = exp / 10 + '0';
	*s++ = exp % 10 + '0';
	*ps = s;
}

/**************************
 * Trim trailing 0s and decimal point from string.
 */

char __ss * _pascal __trim0(char __ss *s)
{   while (*(s-1) == '0')
	s--;
    if (*(s-1) == '.')
	s--;
    return s;
}

