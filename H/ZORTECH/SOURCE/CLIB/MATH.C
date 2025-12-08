/*_math.c   Sat Apr 15 1989   Modified by: Phil Hinger */
/* Copyright (C) 1985-1991 by Walter Bright	*/
/* All Rights Reserved				*/

/* Algorithms from Cody and Waite, "Software Manual For The Elementary	*/
/* Functions".								*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<fltpnt.h>
#include	<errno.h>
#include	<math.h>
#include	<float.h>
#include	<fltenv.h>


#define DTYPE_OTHER	0
#define DTYPE_ZERO	1
#define DTYPE_INFINITE	2
#define DTYPE_SNAN	3
#define DTYPE_QNAN	4


extern int _8087;
extern int __dtype(double);

#if _MT
extern void _pascal __FEEXCEPT(int e);
#define feexcept(e)	(__FEEXCEPT(e))
#else
extern fenv_t _fe_cur_env;
#define feexcept(e)	(_fe_cur_env.status |= (e))
#endif

double __trigerr(int type,double retval,double x,int flag);
double _arcerr(int flag,double x);
double __matherr(int type, double retval, double arg1, double arg2,char * name);
double _largestNaN(double ,double);
double _AsQNaN(double);

/* Functions from math87.asm	*/
double _sqrt87(double);
double _poly87(double, int, double []);
double fabs(double);
double _chop87(double);
double _floor87(double);
double _ceil87(double);
double _fmod87(double,double);
double _atan287(double,double);
double _asincos87(int,double);
double _sincostan87(double,int);
double _log87(double);
double _log1087(double);
double _exp87(double);
double _pow87(double,double,int);


/********* Various math constants **********/

#if SQRT
#if 0
#define SQRTPNT5	 .70710678118654752440
#else
/* From Cody+Waite pg. 24: octal .55202 36314 77473 63110 */
long sqrtpnt5[2] = {0x667f3bcc,0x3fe6a09e};

#endif

#else
extern long sqrtpnt5[2];
#endif
#define SQRTPNT5	(*(double *)sqrtpnt5)

#define SQRT2		1.41421356227309504880
#define SQRT3		1.73205080756887729353
#define LOG10E		0.43429448190325182765
#define LN3OVER2	0.54930614433405484570
#define PIOVER2		1.57079632679489661923
#define PIOVER3		1.04719755119659774615
#define PIOVER4		0.78539816339744830962
#define PIOVER6		0.52359877559829887308
#define THREEPIOVER4	PIOVER4 * 3.0
#define ONEOVERPI	0.31830988618379067154
#define TWOOVERPI	0.63661977236758134308



static long infinite[2]  = {0x00000000, 0x7ff00000};
#define INFINITE    (*(double *)infinite)


/********** Parameters of floating point stuff **************/

#define DSIG		53		/* bits in significand		*/
#define DEXP		11		/* bits in exponent		*/
#define DMAXEXP		(1 << (DEXP - 1))	/* maximum exponent	*/
#define DMINEXP		(-DMAXEXP + 1)		/* minimum exponent	*/

			/* maximum power of 2 exactly representable	*/
			/* which is B**t				*/
#define	MAXPOW2		((double)(1L << 30) * (1L << (DSIG - 1 - 30)))
#define SQRTMAXPOW2	((1L << DSIG/2) * SQRT2)	/* B**(t/2)	*/
#define EPS		(1.0 / SQRTMAXPOW2)		/* B**(-t/2)	*/
#define BIGX		(LN2 * (1 << (DEXP - 1)))
#define SMALLX		(LN2 * -(1 << (DEXP - 1)))


#define DBLEXPMASK	0x7FF0		/* mask for long exponent	*/
#define SIGNMASK	0x8000		/* mask for sign bit		*/

/* Return pointer to the 4 unsigned shorts in a double	*/
#define P(d)	((unsigned short *)(&d))

/* Return biased exponent	*/
#define DOUBLE_EXP(d)  (P(d)[3] & DBLEXPMASK)

/* Return OR'd together bits of significand	*/
#define DOUBLE_SIG(d)  (P(d)[3] & ~(SIGNMASK | DBLEXPMASK) | P(d)[2] | P(d)[1] | P(d)[0])

#define signOfDouble(x)  ((P(x)[3] & SIGNMASK) ? 1 : 0)

/* Turn a signalling NaN into a quiet NaN in place	*/
#define toqnan(x)	(P(x)[3] |= 8)

#ifdef _TRIGERR

/*************************
 */

double __trigerr(int type,double retval,double x,int flag)
{ struct exception e;
  static char _near *name[] = {"sin","cos","tan","sqrt","exp","tanh"};

  e.retval = retval;
  e.type = type;
  e.arg1 = x;
  e.name = name[flag];
  matherr(&e);
  return e.retval;
}


double __matherr(int type,double retval,double arg1,double arg2,char *name)
{
  struct exception e;

  if (isnan(retval))
	toqnan(retval);			/* convert to quiet NaN		*/
  e.retval = retval;
  e.type = type;
  e.arg1 = arg1;
  e.arg2 = arg2;
  e.name = name;
  matherr(&e);
  return e.retval;
}

#endif /* _TRIGERR */

#ifdef MATH1

/*************************
 */

double _arcerr(int flag,double x)
{
	struct exception e;


	e.arg1 = x;
	e.name = (flag) ? "acos" : "asin";
	e.type = DOMAIN;
	e.retval = NAN;
	matherr(&e);
	return e.retval;
}

/*************************
 * Absolute value of double.
 */

#if 0			/* in math87.asm	*/
double fabs(x)
double x;
{
  return (x < 0) ? -x : x;
}
#endif


/***********************************
 * Evaluate polynomial of the form:
 *	(coeff[deg] * x + coeff[deg - 1]) * x + ... + coeff[0]
 */

double poly(double x,int deg,double *coeff)
{	double r;

	if (_8087)
		return _poly87(x,deg,coeff);
	r = coeff[deg];
	while (deg--)
		r = x * r + coeff[deg];
	return r;
}

#endif /* MATH1	*/

#ifdef TRIG
/***********************
 * Do sine and cosine.
 * Input:
 *	cosine = 0:	sine
 *		 1:	cosine
 */

static double near pascal _sincos(double,int);

float sinf(float x)
{

	return (float)_sincos((double)x,0);
}

double sin(double x)
{

	return _sincos(x,0);
}

float cosf(float x)
{

	return (float)_sincos((double)x,1);
}

double cos(double x)
{

	return _sincos(x,1);
}

static double near pascal _sincos(double x,int cosine)
{ double y,xn,f;
  int sgn;
  long n;
  static double r[8] =
  {	-0.16666666666666665052e+0,
	 0.83333333333331650314e-2,
	-0.19841269841201840457e-3,
	 0.27557319210152756119e-5,
	-0.25052106798274584544e-7,
	 0.16058936490371589114e-9,
	-0.76429178068910467734e-12,
	 0.27204790957888846175e-14	/* pg. 138		*/
  };
  static double C1 = 3.1416015625;
  static double C2 = -8.908910206761537356617e-6;	/* pg. 136	*/

  switch(__dtype(x))
  {
     case DTYPE_ZERO	    :
			      if(cosine)
				return 1.0;
     case DTYPE_QNAN	    :
			      return x;
     case DTYPE_INFINITE    :

			      return __trigerr(DOMAIN,NAN,x,cosine);
     case DTYPE_SNAN	    :
			      return __trigerr(DOMAIN,_AsQNaN(x),x,cosine);
  }
  if (_8087)
	return _sincostan87(x,cosine);
  sgn = 1;
  if (x < 0)
  {	y = -x;
	if (!cosine)
		sgn = -1;
  }
  else
	y = x;
  if (cosine)
	y += PIOVER2;

#define YMAX	(PI * MAXPOW2)		/* pi*B**t			*/

  if (y >= YMAX)			/* if total loss of significance */
	return __trigerr(TLOSS,0.0,x,cosine);
  n = y * ONEOVERPI + .5;		/* round and take integer part	*/
  xn = n;
  if (n & 1)				/* if n is odd			*/
	sgn = -sgn;
  if (cosine)
	xn -= .5;
  f = (fabs(x) - xn * C1) - xn * C2;

  if (fabs(f) >= EPS)
  {	double g;

	g = f * f;
	g = g * poly(g,7,r);
	f = f + f * g;
  }
  if (sgn < 0)
	f = -f;
  if (y >= PI * SQRTMAXPOW2)		/* partial loss of significance	*/
	f = __trigerr(PLOSS,f,x,cosine);
  feexcept(FE_INEXACT);
  return f;
}

/******************
 * Compute tan(x).
 */

float tanf(float x)
{
  return (float)tan((double)x);
}

double tan(double x)
{ double y,xn,f,g,xnum,xden,result;
  int sgn = 0;
  long n;
  static double C1 = 1.57080078125;		/* octal 1.4442		*/
  static double C2 = -4.454455103380768678308e-6;
  static double p[3] =
  {	-0.13338350006421960681e+0,
	 0.34248878235890589960e-2,
	-0.17861707342254426711e-4
  };
  static double q[5] =
  {	 0.10000000000000000000e+1,
	-0.46671683339755294240e+0,
	 0.25663832289440112864e-1,
	-0.31181531907010027307e-3,
	 0.49819433993786512270e-6
  };

  y = x;
  switch(__dtype(x))
  {
     case DTYPE_ZERO	    :
     case DTYPE_QNAN	    :
			      return x;
     case DTYPE_INFINITE    :

			      return __trigerr(DOMAIN,NAN,x,2);
     case DTYPE_SNAN	    :
			      return __trigerr(DOMAIN,_AsQNaN(x),x,2);
  }
  if (_8087)
	return _sincostan87(x,2);
  if (x < 0)
  {	y = -x;
	sgn++;
  }
  if (y > SQRTMAXPOW2 * PIOVER2)
	return __trigerr(TLOSS,0.0,x,2);
  n = x * TWOOVERPI + .5;
  xn = n;
  f = (x - xn * C1) - xn * C2;
  if (fabs(f) < EPS)
  {	xnum = f;
	xden = 1.0;
  }
  else
  {	g = f * f;
	xnum = poly(g,2,p) * g * f + f;
	xden = poly(g,4,q);
  }
  if (n & 1)				/* if n is odd			*/
	result = -xden / xnum;
  else
	result = xnum / xden;
  feexcept(FE_INEXACT);
  return result;
}

#endif /* TRIG */

#ifdef SQRT

/*****************************
 * Compute square root of double.
 * Returns:
 *	square root of |x|
 */

#undef sqrtf				/* disable macro versions	*/
#undef sqrt

float sqrtf(float x)
{
  return (float)sqrt((double)x);
}

double sqrt(double x)
{ double f,y;
  int exp,i;				/* exponent			*/
  struct exception e;

  switch(__dtype(x))
  {
     case DTYPE_ZERO	    :
     case DTYPE_QNAN	    : return x;
     case DTYPE_INFINITE    :
			      if (x > 0.0)
				return x;
			      x = NAN;
     case DTYPE_SNAN	    :
			      return __trigerr(DOMAIN, _AsQNaN(x), x, 3);
  }
  if (x < 0)				/* sqrt of negative number?	*/
    return __trigerr(DOMAIN, NAN, x, 3);
  if (_8087)				/* if 8087 is available		*/
	return _sqrt87(x);		/* use the 8087			*/
  f = frexp(x,&exp);			/* split x into f and exp	*/
  y = .41731 + .59016 * f;		/* first guess (y0)	*/
  y = y + f / y;
  y = ldexp(y,-2) + f / y;		/* now we have y2		*/
  y = ldexp(y + f / y,-1);		/* and now y3			*/
  if (exp & 1)				/* if odd exponent		*/
  {
	y *= SQRTPNT5;
  }
  y = ldexp(y,(exp + 1) >> 1);
  feexcept(FE_INEXACT);
  return y;
}

/*****************************
 * Hypoteneuse
 */

double hypot(double x,double y)
{
	return sqrt(x * x + y * y);
}

#endif /* SQRT */

#ifdef LOG

/**************************
 * Compute natural log plus 1
 */

float log1pf(float x)
{

  if (isfinite(x))
    x += 1.0;
  return (float)log((double)x);
}

double log1p(double x)
{

  if (isfinite(x))
    x += 1.0;
  return log(x);
}

/**************************
 * Compute logarithm of x.
 * Input:
 *	log10 = 0	compute natural log
 *		1	compute log base 10
 */

static double _near _pascal _loglog(int log10,double x);


float logf(float x)
{
	return (float)_loglog(0,(double)x);
}

double log(double x)
{
	return _loglog(0,x);
}

float log10f(float x)
{
	return (float)_loglog(1,(double)x);
}

double log10(double x)
{
	return _loglog(1,x);
}

static double _near _pascal _loglog(int log10,double x)
{ double z;
  static char _near *name[] = {"log","log10"};
  int n;
  struct exception e;
  static double C1 = 355./512.;		/* octal .543			*/
  static double C2 = -2.121944400546905827679e-4;
  static double a[3] =
  {	-0.64124943423745581147e+2,
	 0.16383943563021534222e+2,
	-0.78956112887491257267e+0
  };
  static double b[4] =
  {	-0.76949932108494879777e+3,
	 0.31203222091924532844e+3,
	-0.35667977739034646171e+2,
	 0.10000000000000000000e+1
  };

  switch(__dtype(x))
  {
     case DTYPE_ZERO	    :
			      return __matherr(DIVIDE_BY_ZERO, -INFINITY, x, 0.0, name[log10]);
     case DTYPE_INFINITE    :
			      return (x > 0 ) ? x : __matherr(DOMAIN, NAN, x, 0.0, name[log10]);
     case DTYPE_SNAN	    :
			      return __matherr(DOMAIN, x, x, 0.0, name[log10]);
     case DTYPE_QNAN	    :
			      return x;
  }
  if (x < 0 )
    return __matherr(DOMAIN, NAN, x, 0.0, name[log10]);
  if (x == 1.0)
    return 0.0;
  if (_8087)
    return (log10) ? _log1087(x) : _log87(x);
  z = frexp(x,&n);			/* x = z * (2 ** n)		*/
  x = z - .5;
  if (z > SQRTPNT5)
	x -= .5;
  else
  {	z = x;
	n--;
  }
  z = x /(ldexp(z,-1) + .5);
  x = z * z;
  z += z * x * poly(x,2,a) / poly(x,3,b);
  x = n;
  x = (x * C2 + z) + x * C1;
  feexcept(FE_INEXACT);
  return (log10) ? x * LOG10E : x;
}

/*****************************
 * Compute e**x -1.
 */

float expm1f(float x)
{

  x =  (float)exp((double)x);
  return (isfinite(x)) ? x -1.0 : x;
}

double expm1(double x)
{

  x =  exp(x);
  return (isfinite(x)) ?  x -1.0 : x;
}


/*****************************
 * Compute e**x.
 */

float expf(float x)
{
  return (float)exp((double)x);
}

double exp(double x)
{ int n;
  double z;
  struct exception e;
  static double C1 = 355./512.;			/* .543 octal		*/
  static double C2 = -2.121944400546905827679e-4;
  static double p[3] =
  {	0.25000000000000000000e+0,
	0.75753180159422776666e-2,
	0.31555192765684646356e-4
  };
  static double q[4] =
  {	0.50000000000000000000e+0,
	0.56817302698551221787e-1,
	0.63121894374398503557e-3,
	0.75104028399870046114e-6
  };


  switch(__dtype(x))
  {
     case DTYPE_ZERO	    :
			      return 1.0;
     case DTYPE_QNAN	    :
			      return x;
     case DTYPE_INFINITE    :
			      return (x > 0) ? x : 0.0;
     case DTYPE_SNAN	    :
			      return __trigerr(DOMAIN, _AsQNaN(x), x, 4);
  }
  if (_8087)
	return _exp87(x);

#define EPS1	(1.0 / (MAXPOW2 * 2))		/* 2 ** (-b - 1)	*/
  if (fabs(x) < EPS1)
	return 1.0;
  n = x * LOG2E + .5;
  z = n;


  x = (x - z * C1) - z * C2;
  z = x * x;
  x *= poly(z,2,p);
  feexcept(FE_INEXACT);
  return ldexp(.5 + x / (poly(z,3,q) - x),n + 1);
}

/**************************************
 * Compute x ** y.
 * Error if:
 *	x == 0 and y <= 0
 *	x < 0 and y is not integral
 */

float powf(float x,float y)
{
  return (float)pow((double)x,(double)y);
}

static int _near _pascal yodd(double y)	/* determine if y is odd	*/
{
    int oldStatus;
    int yIsOddInteger;
    double yTemp;

    oldStatus = fetestexcept(FE_ALL_EXCEPT);
    yTemp = truncf(y);
    yIsOddInteger = ((yTemp == y) && (fabs(remainder(yTemp,2.0)) == 1.0));
    feclearexcept(FE_ALL_EXCEPT);
    feexcept(oldStatus);
    return yIsOddInteger;
}

double pow(double x,double y)
{ int sgn;
  int pp,p,iw1,mp,m;
  int yIsOddInteger;
  char name[] = "pow";
  struct exception e;
  double v,z,W,W1,W2,Y1,Y2,g,R,U1,U2;
  static double K = 0.44269504088896340736e+0;
  static double P[4] =
  {	0.83333333333333211405e-1,
	0.12500000000503799174e-1,
	0.22321421285924258967e-2,
	0.43445775672163119635e-3
  };
  static double q[7] =
  {	0.69314718055994529629e+0,
	0.24022650695905937056e+0,
	0.55504108664085595326e-1,
	0.96181290595172416964e-2,
	0.13333541313585784703e-2,
	0.15400290440989764601e-3,
	0.14928852680595608186e-4
  };
  static long a[17][2] =		/* assume 2 longs make a double	*/
  {	/* These coefficients are from the octal ones in C+W. The	*/
	/* program makea.c created the hex equivalents.			*/
	0x00000000,0x3ff00000,	/* 1.	*/
	0xa2a490da,0x3feea4af,	/* 0.95760328069857365	*/
	0xdcfba487,0x3fed5818,	/* 0.91700404320467114	*/
	0xdd85529c,0x3fec199b,	/* 0.87812608018664963	*/
	0x995ad3ad,0x3feae89f,	/* 0.84089641525371447	*/
	0x82a3f090,0x3fe9c491,	/* 0.80524516597462714	*/
	0x422aa0db,0x3fe8ace5,	/* 0.7711054127039703	*/
	0x73eb0187,0x3fe7a114,	/* 0.7384130729697496	*/
	0x667f3bcd,0x3fe6a09e,	/* 0.70710678118654755	*/
	0xdd485429,0x3fe5ab07,	/* 0.6771277734684463	*/
	0xd5362a27,0x3fe4bfda,	/* 0.64841977732550475	*/
	0x4c123422,0x3fe3dea6,	/* 0.62092890603674195	*/
	0x0a31b715,0x3fe306fe,	/* 0.59460355750136049	*/
	0x6e756238,0x3fe2387a,	/* 0.56939431737834578	*/
	0x3c7d517b,0x3fe172b8,	/* 0.54525386633262877	*/
	0x6cf9890f,0x3fe0b558,	/* 0.52213689121370681	*/
	0x00000000,0x3fe00000	/* 0.5	*/
  };

#define A(i)	(*(double *)&a[i][0])
#define A1(j)	A(-(1 - (j)))
#define A2(j)	(A(-(1 - 2 * (j))) - A1(2 * (j)))

  switch( __dtype(x)* 5 + __dtype(y))
  {
   /* Other and */

/* Other    */ case 0 :    break;
/* Zero     */ case 1 :    return 1.0;
/* Infinite */ case 2 :
			   if( fabs(x) == 1.0)
			     return __matherr(DOMAIN,NAN,x,y,name);
			   if(y > 0.0)
			     return (fabs(x) > 1.0) ? INFINITY : 0.0;
			   else
			     return (fabs(x) < 1.0) ? INFINITY : 0.0;
/* SNaN     */ case 3 :    return __matherr(DOMAIN,y,x,y,name);
/* QNaN     */ case 4 :    return  y;

   /* Zero and */
/* Other    */ case 5 :
			   yIsOddInteger = yodd(y);
			   if(y > 0.0)
			     return (yIsOddInteger) ? x : 0.0;
			   else
			     return __matherr(DIVIDE_BY_ZERO,(yIsOddInteger) ? copysign(INFINITY,x): INFINITY,x,y,name);

/* Zero     */ case 6 :    return 1.0;
/* Infinite */ case 7 :    return (y > 0.0) ? 0.0 :__matherr(DIVIDE_BY_ZERO,INFINITY,x,y,name);
/* SNaN     */ case 8 :    return __matherr(DOMAIN,y,x,y,name);
/* QNaN     */ case 9 :    return  y;

   /* Infinite and */
/* Other    */ case 10:
			   yIsOddInteger = yodd(y);
			   if(x > 0.0)
			     return (y > 0) ? INFINITY : 0.0;
			   else
			     if (y > 0)
			       return (yIsOddInteger) ? -INFINITY : INFINITY ;
			     else
			       return (yIsOddInteger) ? -0.0  : 0.0 ;

/* Zero     */ case 11:     return 1.0;
/* Infinite */ case 12:     return (y > 0.0) ? INFINITY : 0.0 ;
/* SNaN     */ case 13:     return __matherr(DOMAIN,y,x,y,name);
/* QNaN     */ case 14:     return  y;

   /* SNaN and */
/* Other    */ case 15:     return __matherr(DOMAIN,x,x,y,name);
/* Zero     */ case 16:     return 1.0;
/* Infinite */ case 17:     return __matherr(DOMAIN,x,x,y,name);
/* SNaN     */ case 18:     return __matherr(DOMAIN,_largestNaN(x,y),x,y, name);
/* QNaN     */ case 19:     return __matherr(DOMAIN,y,x,y,name);

   /* QNaN and */
/* Other    */ case 20:    return  x;
/* Zero     */ case 21:    return  1.0;
/* Infinite */ case 22:    return  x;
/* SNaN     */ case 23:    return __matherr(DOMAIN,x,x,y,name);
/* QNaN     */ case 24:    return _largestNaN(x,y);
  }
  if (y == 1) return x;
  sgn = 0;
  if (x <= 0)
  {	if (x == 0)
	{	if (y <= 0)
			goto errdom;
		return 0.0;
	}
	/* see if y is integral	*/
	if (y >= (long) 0x80000000L &&
	    y <= (long) 0x7FFFFFFFL &&
	    (double)(long)y == y)
	{	x = -x;
		sgn = (long)y & 1;
	}
	else /* error */
		goto errdom;
  }
  if (_8087)
	return _pow87(x,y,sgn);
  g = frexp(x,&m);
  p = 1;
  if (g <= A1(9)) p = 9;
  if (g <= A1(p + 4)) p += 4;
  if (g <= A1(p + 2)) p += 2;
  z = ((g - A1(p + 1)) - A2((p + 1) >> 1)) / (g + A1(p + 1));
  z += z;
  v = z * z;
  R = z * v * poly(v,3,P);
  R += K * R;
  U2 = R + z * K;
  U2 += z;
#define REDUCE(V) ldexp((double)(long)(16.0 * (V)),-4)
  U1 = ldexp((double)(m * 16 - p),-4);
  Y1 = REDUCE(y);
  Y2 = y - Y1;
  W = U2 * y + U1 * Y2;
  W1 = REDUCE(W);
  W2 = W - W1;
  W = W1 + U1 * Y1;
  W1 = REDUCE(W);
  W2 += W - W1;
  W = REDUCE(W2);
  iw1 = 16 * (W1 + W);
  W2 -= W;
  if (iw1 >= 16 * DMAXEXP)
  {	e.type = OVERFLOW;
	e.retval = sgn ? -DBL_MAX : DBL_MAX;
	goto err;
  }
  if (iw1 <= 16 * DMINEXP)
  {	e.type = UNDERFLOW;
	goto err0;
  }
  if (W2 > 0)				/* next step requires W2 <= 0	*/
  {	iw1++;
	W2 -= .0625;
  }
  mp = iw1 / 16 + ((iw1 < 0) ? 0 : 1);
  pp = 16 * mp - iw1;
  z = W2 * poly(W2,6,q);
  z = A1(pp + 1) + A1(pp + 1) * z;
  z = ldexp(z,mp);
  feexcept(FE_INEXACT);
  return sgn ? -z : z;

errdom:
  e.type = DOMAIN;
err0:
  e.retval = 0.0;
err:
  e.name = name;
  e.arg1 = x;
  e.arg2 = y;
  matherr(&e);
  return e.retval;
}

#endif /* LOG */

#ifdef ATRIG

/*************************
 * Compute asin(x) (flag == 0) or acos(x) (flag == 1).
 */

static double _near _pascal _asincos(int,double);

float asinf(float x)
{
	return (float)_asincos(0,(double)x);
}

double asin(double x)
{
	return _asincos(0,x);
}

float acosf(float x)
{
   return (double)_asincos(1,(double)x);
}

double acos(double x)
{
	return _asincos(1,x);
}

static double _near _pascal _asincos(int flag,double x)
{	double y,r,g,result;
	int i,dtype;
	static double p[5] =
	{	-0.27368494524164255994e+2,
		 0.57208227877891731407e+2,
		-0.39688862997504877339e+2,
		 0.10152522233806463645e+2,
		-0.69674573447350646411e+0
	};
	static double q[6] =
	{	-0.16421096714498560795e+3,
		 0.41714430248260412556e+3,
		-0.38186303361750149284e+3,
		 0.15095270841030604719e+3,
		-0.23823859153670238830e+2,
		 0.10000000000000000000e+1
	};
	static double a[2] = {	0.0,PIOVER4	};
	       static double b[2] = {  PIOVER2,PIOVER4 };

	switch( __dtype(x))
	{
	  case DTYPE_QNAN:
			    return x;
	  case DTYPE_SNAN:
			    toqnan(x);
			    return _arcerr(flag,x);
	  case DTYPE_ZERO:
			    if (flag)
			    {
			      feexcept(FE_INEXACT);
			      return PIOVER2;
			    }
			    else
			      return x;
	}
	if(x == 1.0)
	  if(flag)
	    return 0.0;
	  else
	  {
	    feexcept(FE_INEXACT);
	    return PIOVER2;
	  }
	if(x == -1.0)
	{
	  feexcept(FE_INEXACT);
	  return (flag) ? PI : -PIOVER2;
	}
	if (_8087)
		return _asincos87(flag,x);
	y = fabs(x);
	if (y > .5)
	{
		if (y > 1.0)
		{
			return _arcerr(flag,x);
		}
		i = 1 - flag;
		g = ldexp(1. - y,-1);		/* g = (1-y)/2		*/
		y = -ldexp(sqrt(g),1);		/* y = -2 * g ** .5	*/
		goto L1;
	}
	else
	{	i = flag;
		if (y < EPS)
			result = y;
		else
		{	g = y * y;
		    L1:	r = g * poly(g,4,p) / poly(g,5,q);
			result = y + y * r;
		}
	}
	if (flag == 0)				/* if asin(x)		*/
	{	result += a[i];
		result += a[i];
		if (x < 0)
			result = -result;
	}
	else
	{	if (x < 0)
		{	result += b[i];
			result += b[i];
		}
		else
		{	result = a[i] - result;
			result += a[i];
		}
	}
	feexcept(FE_INEXACT);
	return result;
}

/****************
 * Compute arctan(V / U).
 */

float atanf(float x)
{
	return (float)atan2((double)x,1.0);
}

double atan(double x)
{
	return atan2(x,1.0);
}

float atan2f(float V,float U)
{
  return (float)atan2((double)V,(double)U);
}
double atan2(double V,double U)
{	int oldStatus,newStatus;
	int n;
	double result,g,f;
	struct exception e;
	static double p[4] =
	{	-0.13688768894191926929e+2,
		-0.20505855195861651981e+2,
		-0.84946240351320683534e+1,
		-0.83758299368150059274e+0
	};
	static double q[5] =
	{	 0.41066306682575781263e+2,
		 0.86157349597130242515e+2,
		 0.59578436142597344465e+2,
		 0.15024001160028576121e+2,
		 0.10000000000000000000e+1
	};
	static double a[4] =
	{	0.0, PIOVER6, PIOVER2, PIOVER3	};
#if 0
#define TWOMINUSSQRT3	0.26794919243112270647
#else
/* From Cody+Waite pg. 204: octal 0.21114 12136 47546 52614	*/
static long twominussqrt3[2] = {0x5e9ecd56,0x3fd12614};
#define TWOMINUSSQRT3	(*(double *)twominussqrt3)
#endif

  switch(50 * signOfDouble(V) + 25 * signOfDouble(U) + __dtype(V)* 5 + __dtype(U))
  {
    /* +V and + U */

   /* Other and */

/* Other    */ case 0 :    break;
/* Zero     */ case 1 :    feexcept(FE_INEXACT); return PIOVER2;
/* Infinite */ case 2 :    return 0.0;
/* SNaN     */ case 3 :    result = U; goto err;
/* QNaN     */ case 4 :    return  U;

   /* Zero and */
/* Other    */ case 5 :
/* Zero     */ case 6 :
/* Infinite */ case 7 :    return 0.0;
/* SNaN     */ case 8 :    result = U; goto err;
/* QNaN     */ case 9 :    return  U;

   /* Infinite and */
/* Other    */ case 10:
/* Zero     */ case 11:    feexcept(FE_INEXACT);return PIOVER2;
/* Infinite */ case 12:    feexcept(FE_INEXACT);return PIOVER4;
/* SNaN     */ case 13:    result = U; goto err;
/* QNaN     */ case 14:    return  U;

   /* SNaN and */
/* Other    */ case 15:
/* Zero     */ case 16:
/* Infinite */ case 17:    result = V; goto err;
/* SNaN     */ case 18:    result = _largestNaN(V,U); goto err;
/* QNaN     */ case 19:    result = V; goto err;

   /* QNaN and */
/* Other    */ case 20:
/* Zero     */ case 21:
/* Infinite */ case 22:    return V;
/* SNaN     */ case 23:    result = V; goto err;
/* QNaN     */ case 24:    return _largestNaN(V,U);

    /* +V and -U */

   /* Other and */

/* Other    */ case 25:     break;
/* Zero     */ case 26:     feexcept(FE_INEXACT);return PIOVER2;
/* Infinite */ case 27:     feexcept(FE_INEXACT);return PI;
/* SNaN     */ case 28:     result = U; goto err;
/* QNaN     */ case 29:     return  U;

   /* Zero and */
/* Other    */ case 30:
/* Zero     */ case 31:
/* Infinite */ case 32:     feexcept(FE_INEXACT); return PI;
/* SNaN     */ case 33:     result = U; goto err;
/* QNaN     */ case 34:     return  U;

   /* Infinite and */
/* Other    */ case 35:
/* Zero     */ case 36:     feexcept(FE_INEXACT); return PIOVER2;
/* Infinite */ case 37:     feexcept(FE_INEXACT); return THREEPIOVER4;
/* SNaN     */ case 38:     result = U; goto err;
/* QNaN     */ case 39:     return  U;

   /* SNaN and */
/* Other    */ case 40:
/* Zero     */ case 41:
/* Infinite */ case 42:     result = V; goto err;
/* SNaN     */ case 43:     result = _largestNaN(V,U); goto err;
/* QNaN     */ case 44:     result = U; goto err;

   /* QNaN and */
/* Other    */ case 45:
/* Zero     */ case 46:
/* Infinite */ case 47:    return  V;
/* SNaN     */ case 48:    result = U; goto err;
/* QNaN     */ case 49:    return _largestNaN(V,U);

    /* -V and + U */

   /* Other and */

/* Other    */ case 50:    break;
/* Zero     */ case 51:    feexcept(FE_INEXACT);return -PIOVER2;
/* Infinite */ case 52:    return -0.0;
/* SNaN     */ case 53:    result = U; goto err;
/* QNaN     */ case 54:    return U;

   /* Zero and */
/* Other    */ case 55:
/* Zero     */ case 56:
/* Infinite */ case 57:    return -0.0;
/* SNaN     */ case 58:    result = U; goto err;
/* QNaN     */ case 59:    return  U;

   /* Infinite and */
/* Other    */ case 60:
/* Zero     */ case 61:    feexcept(FE_INEXACT);return -PIOVER2;
/* Infinite */ case 62:    feexcept(FE_INEXACT);return -PIOVER4;
/* SNaN     */ case 63:    result = U; goto err;
/* QNaN     */ case 64:    return U;

   /* SNaN and */
/* Other    */ case 65:
/* Zero     */ case 66:
/* Infinite */ case 67:    result = V; goto err;
/* SNaN     */ case 68:    result = _largestNaN(V,U); goto err;
/* QNaN     */ case 69:    result = U; goto err;

   /* QNaN and */
/* Other    */ case 70:
/* Zero     */ case 71:
/* Infinite */ case 72:    return  V;
/* SNaN     */ case 73:    result = V; goto err;
/* QNaN     */ case 74:    return _largestNaN(V,U);

    /* -V and -U */

   /* Other and */

/* Other    */ case 75:    break;
/* Zero     */ case 76:    feexcept(FE_INEXACT);return -PIOVER2;
/* Infinite */ case 77:    feexcept(FE_INEXACT);return -PI;
/* SNaN     */ case 78:    result = U; goto err;
/* QNaN     */ case 79:    return U;

   /* Zero and */
/* Other    */ case 80:
/* Zero     */ case 81:
/* Infinite */ case 82:    feexcept(FE_INEXACT);return -PI;
/* SNaN     */ case 83:    result = U; goto err;
/* QNaN     */ case 84:    return  U;

   /* Infinite and */
/* Other    */ case 85:
/* Zero     */ case 86:    feexcept(FE_INEXACT);return -PIOVER2;
/* Infinite */ case 87:    feexcept(FE_INEXACT);return -THREEPIOVER4;
/* SNaN     */ case 88:    result = U; goto err;
/* QNaN     */ case 89:    return  U;

   /* SNaN and */
/* Other    */ case 90:
/* Zero     */ case 91:
/* Infinite */ case 92:     result = V;			goto err;
/* SNaN     */ case 93:     result = _largestNaN(V,U);	goto err;
/* QNaN     */ case 94:     result = U;			goto err;

   /* QNaN and */
/* Other    */ case 95:
/* Zero     */ case 96:
/* Infinite */ case 97:    return  V;
/* SNaN     */ case 98:    result = V; goto err;
/* QNaN     */ case 99:    return _largestNaN(V,U);
  }

	if (_8087)
		return _atan287(V,U);

	oldStatus = fetestexcept(FE_ALL_EXCEPT);
	feclearexcept(FE_ALL_EXCEPT);
	f = fabs(V / U);
	newStatus = fetestexcept(FE_ALL_EXCEPT);
	feexcept(oldStatus);

	if (newStatus & FE_OVERFLOW )	      /* then overflow on V/U */
	{
		result = PIOVER2;
		goto C;
	}
	if (newStatus & FE_UNDERFLOW)	     /* then underflow on V/U */
	{
		result = 0.0;
		goto B;
	}
	if (f > 1.0)
	{	f = 1 / f;
		n = 2;
	}
	else
		n = 0;
	if (f > TWOMINUSSQRT3)
	{	double tmp;	/* to make sure of order of evaluation	*/

		tmp = (SQRT3 - 1) * f - 0.5;
		tmp -= 0.5;
		f = (tmp + f) / (SQRT3 + f);
		n++;
	}
	if (fabs(f) < EPS)
		result = f;
	else
	{	g = f * f;
		result = (g * poly(g,3,p)) / poly(g,4,q);
		result = f + f * result;
	}
	if (n > 1)
		result = -result;
	result += a[n];
    B:	if (U < 0.0)
		result = PI - result;
    C:	if (V < 0.0)
		result = -result;
	feexcept(FE_INEXACT);
	return result;

    err:
	return __matherr(DOMAIN,result,V,U,"atan2");
}

#endif /* ATRIG */

#ifdef HYPER

/************************
 * sinh(x) and cosh(x).
 *	sinh = 1:	sinh()
 *	       0:	cosh()
 */

static double _near _pascal _sinhcosh(int sinh,double x);

float sinhf(float x)
{
	return (float)_sinhcosh(1,(double)x);
}

double sinh(double x)
{
	return _sinhcosh(1,x);
}

float coshf(float x)
{
	return (float)_sinhcosh(0,(double)x);
}

double cosh(double x)
{
	return _sinhcosh(0,x);
}

static double _near _pascal _sinhcosh(int sinh,double x)
{	double y,z,w,f;
	static char _near *name[] = {"cosh","sinh"};
	int sgn;
	static long lnv[2] =		/* 0.69316 10107 42187 50000e+0	*/
	{0L,(0x3FDL << (52 - 32)) + (0542714L << 3)};	/* 0.542714 octal */
	static double vmin2 =  0.24999308500451499336e+0;
	static double v2min1 = 0.13830277879601902638e-4;
	static double p[4] =
	{	-0.35181283430177117881e+6,
		-0.11563521196851768270e+5,
		-0.16375798202630751372e+3,
		-0.78966127417357099479e+0
	};
	static double q[4] =
	{	-0.21108770058106271242e+7,
		 0.36162723109421836460e+5,
		-0.27773523119650701667e+3,
		 0.10000000000000000000e+1
	};
	switch(__dtype(x))
	{
	  case DTYPE_ZERO	 :
				   if (!sinh)
				      break;

	  case DTYPE_QNAN	 :
				   return x;
	  case DTYPE_INFINITE	 :
				   return (sinh) ? x : INFINITE;
	  case DTYPE_SNAN	 :
				   return __matherr(DOMAIN,x,x,0.0,name[sinh]);
	}
	y = fabs(x);
	sgn = (sinh) ? (x < 0.0) : 0;
	if (!sinh || y > 1.0)
	{	if (y > BIGX)	/* largest value exp() can handle	*/
		{	w = y - *(double *)&lnv[0];
			if (w > BIGX + 0.69 - *(double *)&lnv[0])
			   return __matherr(DOMAIN,x,x,0.0,name[sinh]);
			z = exp(w);
			x = z + v2min1 * z;
		}
		else
		{	x = exp(y);
			x = ldexp((x + ((sinh) ? -(1 / x) : (1 / x))),-1);
		}
		if (sgn)
			x = -x;
	}
	else
	{	if (y >= EPS)		/* EPS = B ** (-t/2)		*/
		{	f = x * x;
			f *= (poly(f,3,p) / poly(f,3,q));
			x += x * f;
		}
	}
	feexcept(FE_INEXACT);
	return x;
}

/*************************
 * Compute tanh(x).
 * No error returns.
 */

float tanhf(float x)
{
    return (float)tanh((double)x);
}

double tanh(double x)
{	double g;
	int sgn;
	static double p[3] =
	{	-0.16134119023996228053e+4,
		-0.99225929672236083313e+2,
		-0.96437492777225469787e+0
	};
	static double q[4] =
	{	0.48402357071988688686e+4,
		0.22337720718962312926e+4,
		0.11274474380534949335e+3,
		0.10000000000000000000e+1
	};

	switch(__dtype(x))
	{
	  case DTYPE_ZERO	 :
	  case DTYPE_QNAN	 :
				   return x;
	  case DTYPE_INFINITE	 :
				   return (x > 0) ? 1.0 : -1.0;
	  case DTYPE_SNAN	 :
				   return __trigerr(DOMAIN,_AsQNaN(x),x,5);
	}
	sgn = 0;
	if (x < 0)
	{	sgn++;
		x = -x;
	}
	if (x > (LN2 + (DSIG + 1) * LN2) / 2)
		x = 1.0;
	else if (x > LN3OVER2)
	{	x = 0.5 - 1.0 / (exp(x + x) + 1.0);
		x += x;
	}
	else if (x < EPS)
		/* x = x */ ;
	else
	{	g = x * x;
		g = (g * poly(g,2,p)) / poly(g,3,q);
		x += x * g;
	}
	feexcept(FE_INEXACT);
	return (sgn) ? -x : x;
}

#endif /* HYPER */


#ifdef ROUND

/***********************************
 * Returns i, which is the smallest integer
 * such that i >= x.
 */

double ceil(double x)
{
#if 1
    double result;
    int old_mode;

    if( _8087)
      return _ceil87(x);

    old_mode = fegetround();
    fesetround(FE_UPWARD);
    result = rint(x);
    fesetround(old_mode);
    return result;
#else
    double i;

    return _8087 ? _ceil87(x) : ((modf(x,&i) <= 0.0) ? i : i + 1);
#endif
}

float ceilf(float x)
{
    float result;
    int old_mode;

    old_mode = fegetround();
    fesetround(FE_UPWARD);
    result = rintf(x);
    fesetround(old_mode);
    return result;
}

/***********************************
 * Returns i, which is the largest integer
 * such that i <= x.
 */

double floor(double x)
{
#if 1
    double result;
    int old_mode;

    if (_8087)
      return _floor87(x);

    old_mode = fegetround();
    fesetround(FE_DOWNWARD);
    result = rint(x);
    fesetround(old_mode);
    return result;
#else
    double i;

    return _8087 ? _floor87(x) : ((modf(x,&i) >= 0.0) ? i : i - 1);
#endif
}

float floorf(float x)
{
    float result;
    int old_mode;

    old_mode = fegetround();

    fesetround(FE_DOWNWARD);

    result = rintf(x);

    fesetround(old_mode);

    return result;
}

/**************************
 * Return the larger of the two NaNs, converting it to a quiet NaN.
 * Only the signficand bits are considered.
 * The sign and QNAN bits are ignored.
 */

double _largestNaN(double x1 ,double x2)
{
#define MSLSIG	0x7FFFF			/* most significant dword significand */

    toqnan(x1);
    toqnan(x2);
    if ((((unsigned long *)&x1)[1] & MSLSIG) != (((unsigned long *)&x2)[1] & MSLSIG))
    {
	if ((((unsigned long *)&x1)[1] & MSLSIG) >= (((unsigned long *)&x2)[1] & MSLSIG))
	    goto retx1;
	else
	    goto retx2;
    }
    else
    {
	if (((unsigned long *)&x1)[0] >= ((unsigned long *)&x2)[0])
	    goto retx1;
	else
	    goto retx2;
    }

retx1:
    return x1;

retx2:
    return x2;
}

/**********************
 * Convert from signalling NaN to quiet NaN.
 */

double _AsQNaN(double x)
{
  P(x)[3] |= 8;
  return x;
}

/**********************
 */

double round(double x)
{
  int oldRound;

  oldRound = fegetround();
  fesetround(FE_TONEAREST);
  x = rint(x);
  fesetround(oldRound);
  return x;
}

float roundf(float x)
{
  int oldRound;

  oldRound = fegetround();
  fesetround(FE_TONEAREST);
  x = rintf(x);
  fesetround(oldRound);
  return x;
}

/********************************
 * Same as rint() except that inexact exception is not raised.
 */

double nearbyint(double x)
{
  fenv_t fe;
  int oldStatus;

  fegetenv(&fe);
  oldStatus = fe.status & FE_INEXACT;
  x = rint(x);
  fegetenv(&fe);
  fe.status = (fe.status & ~FE_INEXACT) | oldStatus;
  fesetenv(&fe);
  return x;
}

float nearbyintf(float x)
{
  fenv_t fe;
  int oldStatus;

  fegetenv(&fe);
  oldStatus = fe.status & FE_INEXACT;
  x = rintf(x);
  fegetenv(&fe);
  fe.status = (fe.status & ~FE_INEXACT) | oldStatus;
  fesetenv(&fe);
  return x;
}

/****************************
 */

double trunc(double x)
{
  int oldRound;

  oldRound = fegetround();
  fesetround(FE_TOWARDZERO);
  x = rint(x);
  fesetround(oldRound);
  return(x);
}

float truncf(float x)
{
  int oldRound;

  oldRound = fegetround();
  fesetround(FE_TOWARDZERO);
  x = rintf(x);
  fesetround(oldRound);
  return(x);
}

/****************************
 */

long int rndtol(long double x)
{
  return ((long int) rint(x));
}

long int rndtonl(long double x)
{
  int oldRound;
  long int y;

  oldRound = fegetround();
  fesetround(FE_TOWARDZERO);
  x += 0.5;
  y = rint(x);
  fesetround(oldRound);
  return(y);
}


/************************************
 * IEEE remainder function.
 * If y != 0 then the remainder r = x REM y is defined as:
 *	r = x - y * n
 */

float remainderf(float y, float x)
{
    int quo;

    return remquof(y, x, &quo);
}
double remainder(double y, double x)
{
    int quo;

    return remquo(y, x, &quo);
}

float remquof(float y, float x, int *quo)
{
  float n,rem;
  int oldRound,oldStatus,classifyX,classifyY;
  extern int _8087;

  *quo = 0;
  classifyX = fpclassify(x);
  if(classifyX == FP_SUBNORMAL)
    classifyX = FP_NORMAL;
  classifyY = fpclassify(y);
  switch(classifyY + 6 * classifyX)
  {
    case  FP_NANS      + 6 * FP_NANS	  : feexcept(FE_INVALID); return _AsQNaN(x);
    case  FP_NANQ      + 6 * FP_NANS	  : feexcept(FE_INVALID); return y;
    case  FP_INFINITE  + 6 * FP_NANS	  :
    case  FP_SUBNORMAL + 6 * FP_NANS	  :
    case  FP_NORMAL    + 6 * FP_NANS	  :
    case  FP_ZERO      + 6 * FP_NANS	  :
					    feexcept(FE_INVALID); return _AsQNaN(x);

    case  FP_NANS      + 6 * FP_NANQ	  : feexcept(FE_INVALID); return x;
    case  FP_NANQ      + 6 * FP_NANQ	  :
    case  FP_INFINITE  + 6 * FP_NANQ	  :
    case  FP_SUBNORMAL + 6 * FP_NANQ	  :
    case  FP_NORMAL    + 6 * FP_NANQ	  :
    case  FP_ZERO      + 6 * FP_NANQ	  :
					    return x;

    case  FP_NANS      + 6 * FP_INFINITE  : feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_INFINITE  : return y;
    case  FP_INFINITE  + 6 * FP_INFINITE  : feexcept(FE_INVALID); return NAN;
    case  FP_SUBNORMAL + 6 * FP_INFINITE  :
    case  FP_NORMAL    + 6 * FP_INFINITE  :
    case  FP_ZERO      + 6 * FP_INFINITE  : return y;

    case  FP_NANS      + 6 * FP_NORMAL	  : feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_NORMAL	  : return y;
    case  FP_INFINITE  + 6 * FP_NORMAL	  : feexcept(FE_INVALID); return NAN;
    case  FP_SUBNORMAL + 6 * FP_NORMAL	  :
    case  FP_NORMAL    + 6 * FP_NORMAL	  :
					   oldStatus = fetestexcept(FE_ALL_EXCEPT);
					   feclearexcept(FE_ALL_EXCEPT);
					   oldRound  = fegetround();
					   fesetround(FE_TONEAREST);
					   n  =  rint( y / x);
					   if (fetestexcept(FE_OVERFLOW))
					     rem = (y < 0.0 ) ? -0.0 : 0.0;
					   else
					   {
					     unsigned long q; /* remember fmod can cause recursive error */

					     feclearexcept(FE_ALL_EXCEPT);
					     q = n;
					     if(!fetestexcept(FE_INVALID ))
					       *quo = q & 7;
					     feclearexcept(FE_ALL_EXCEPT);
					     rem =   y - n * x;
					     if (fetestexcept(FE_OVERFLOW))
					     {
					       float temp;

					       temp = x;
					       y -= copysign(temp,y);
					       n -= copysign(1.0,n);
					       rem = y - n * x;
					     }
					   }
					   if( rem == 0.0)
					     rem = (y < 0.0 ) ? -0.0 : 0.0;
					   feclearexcept(FE_ALL_EXCEPT);
					   feexcept(oldStatus);
					   fesetround(oldRound);
					   return rem;

    case  FP_ZERO      + 6 * FP_NORMAL	  :  return y;

    case  FP_NANS      + 6 * FP_ZERO	  :  feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_ZERO	  :  return y;
    case  FP_INFINITE  + 6 * FP_ZERO	  :
    case  FP_SUBNORMAL + 6 * FP_ZERO	  :
    case  FP_NORMAL    + 6 * FP_ZERO	  :
    case  FP_ZERO      + 6 * FP_ZERO	  :
					  feexcept(FE_INVALID);
					  return (float)NAN;

  }
}

double remquo(double y, double x, int *quo)
{
  double n,rem;
  int oldRound,oldStatus,classifyX,classifyY;

  classifyX = fpclassify(x);
  if(classifyX == FP_SUBNORMAL)
    classifyX = FP_NORMAL;
  classifyY = fpclassify(y);
  switch(classifyY + 6 * classifyX)
  {
    case  FP_NANS      + 6 * FP_NANS	  : feexcept(FE_INVALID); return _AsQNaN(x);
    case  FP_NANQ      + 6 * FP_NANS	  : feexcept(FE_INVALID); return y;
    case  FP_INFINITE  + 6 * FP_NANS	  :
    case  FP_SUBNORMAL + 6 * FP_NANS	  :
    case  FP_NORMAL    + 6 * FP_NANS	  :
    case  FP_ZERO      + 6 * FP_NANS	  :
					    feexcept(FE_INVALID); return _AsQNaN(x);

    case  FP_NANS      + 6 * FP_NANQ	  : feexcept(FE_INVALID); return x;
    case  FP_NANQ      + 6 * FP_NANQ	  :
    case  FP_INFINITE  + 6 * FP_NANQ	  :
    case  FP_SUBNORMAL + 6 * FP_NANQ	  :
    case  FP_NORMAL    + 6 * FP_NANQ	  :
    case  FP_ZERO      + 6 * FP_NANQ	  :
					    return x;

    case  FP_NANS      + 6 * FP_INFINITE  : feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_INFINITE  : return y;
    case  FP_INFINITE  + 6 * FP_INFINITE  : feexcept(FE_INVALID); return NAN;
    case  FP_SUBNORMAL + 6 * FP_INFINITE  :
    case  FP_NORMAL    + 6 * FP_INFINITE  :
    case  FP_ZERO      + 6 * FP_INFINITE  : return y;

    case  FP_NANS      + 6 * FP_NORMAL	  : feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_NORMAL	  : return y;
    case  FP_INFINITE  + 6 * FP_NORMAL	  : feexcept(FE_INVALID); return NAN;
    case  FP_SUBNORMAL + 6 * FP_NORMAL	  :
    case  FP_NORMAL    + 6 * FP_NORMAL	  :
					   oldStatus = fetestexcept(FE_ALL_EXCEPT);
					   feclearexcept(FE_ALL_EXCEPT);
					   oldRound  = fegetround();
					   fesetround(FE_TONEAREST);
					   n  =  rint( y / x);
					   if (fetestexcept(FE_OVERFLOW))
					     rem = (y < 0.0 ) ? -0.0 : 0.0;
					   else
					   {
					     unsigned long q; /* remember fmod can cause recursive error */

					     feclearexcept(FE_ALL_EXCEPT);
					     q = n;
					     if(!fetestexcept(FE_INVALID))
					       *quo = q & 7;
					     feclearexcept(FE_ALL_EXCEPT);
					     rem =   y - n * x;
					     if (fetestexcept(FE_OVERFLOW))
					     {
					       double temp;

					       temp = x;
					       y -= copysign(temp,y);
					       n -= copysign(1.0,n);
					       rem = y - n * x;
					     }
					     if (rem == 0.0)
						rem = (y < 0.0 ) ? -0.0 : 0.0;
					   }
					   feclearexcept(FE_ALL_EXCEPT);
					   feexcept(oldStatus);
					   fesetround(oldRound);
					   return rem;

    case  FP_ZERO      + 6 * FP_NORMAL	  :  return y;

    case  FP_NANS      + 6 * FP_ZERO	  :  feexcept(FE_INVALID); return _AsQNaN(y);
    case  FP_NANQ      + 6 * FP_ZERO	  :  return y;
    case  FP_INFINITE  + 6 * FP_ZERO	  :
    case  FP_SUBNORMAL + 6 * FP_ZERO	  :
    case  FP_NORMAL    + 6 * FP_ZERO	  :
    case  FP_ZERO      + 6 * FP_ZERO	  :
					  feexcept(FE_INVALID);
					  return NAN;

  }
}


/*****************************
 * Remainder of x / y.
 * If y == 0, return 0.
 * Else return f such that x==i*y+f.
 */

float fmodf(float x,float y)
{
    if (_8087)
	return (float)_fmod87((double)x,(double)y);
    else
    {
	float result;

	result = remainderf(fabsf(x), (y = fabsf(y)));
	if(signbit(result))
	   result += y;
	return copysignf(result,x);
    }
}

double fmod(double x,double y)
{
    if (_8087)
	return _fmod87(x,y);
    else
    {
#if 1
	double result;

	result = remainder(fabs(x), (y = fabs(y)));
	if(signbit(result))
	   result += y;
	return copysign(result,x);
#else
	y = fabs(y);
	return (x + y == x) ? 0.0 : modf(x / y,&x) * y;
#endif
    }
}

/***********************************
 * Split x into an integer and fractional part (f).
 * Returns:
 *	f (with same sign as f)
 *	*pi = integer part (with same sign as f)
 * Note that the integer part is stored into a double!
 */

float modff(float value, float *iptr)
{
    int sgn = 0;

    if (_8087)
       *iptr = (float)_chop87((double)value);
    else
    {
	int save_rnd;

	save_rnd = fegetround();
	fesetround(FE_TOWARDZERO);
	*iptr = nearbyintf(value);
	fesetround(save_rnd);
    }
    return copysign(
		    (fabsf(value) == INFINITY) ? 0.0 : value - (*iptr), value
		   );
}
double modf(double value,double *iptr)
{
    int sgn = 0;

#if 1
    if (_8087)
      *iptr = _chop87(value);
    else
    {
	int save_rnd;

	save_rnd = fegetround();
	fesetround(FE_TOWARDZERO);
	*iptr = nearbyint(value);
	fesetround(save_rnd);
    }
      return copysign(
		      (fabs(value) == INFINITY) ? 0.0 : value - (*iptr), value
		     );
#else
    if (_8087)
    {
       *iptr = _chop87(value);
	return value - *iptr;
    }
    else
    {
	*iptr = x;
	if (x < 0)
	{	if (*iptr > -MAXPOW2)
		{	*iptr -= MAXPOW2;
			*iptr += MAXPOW2;
			if (*iptr < x)
				(*iptr)++;
		}
	}
	else
	{	if (*iptr < MAXPOW2)
		{	*iptr += MAXPOW2;
			*iptr -= MAXPOW2;
			if (*iptr > x)
				(*iptr)--;
		}
	}
	return x - *iptr;
    }
#endif
}


float frexpf(float value, int *exp)
{
  return  (float)frexp((double)value,exp);
}

float ldexpf(float value,int exp)
{
  return (float)ldexp((double)value, exp);
}

double scalb(double x, long int n)
{
  if (abs(n) < 32768)
    return ldexp(x,(int)n);
  else
    return (n > 0) ? ldexp(x,32768) : ldexp(x,-32768);
}

float scalbf(float x, long int n)
{
  if (abs(n) < 32768)
    return (float)ldexp((double)x,(int)n);
  else
    return (n > 0) ? (float)ldexp((double)x,32768) : (float)ldexp((double)x,-32768);
}

#endif /* round */


#ifdef IEEE


/********************************
 * Return x with the sign of y.
 */

double copysign(double x, double y)
{
	P(x)[3] &= ~ SIGNMASK;
	P(x)[3] |= P(y)[3] & SIGNMASK;
	return x;
}

float copysignf(float x, float y)
{
	P(x)[1] &= ~ SIGNMASK;
	P(x)[1] |= P(y)[1] & SIGNMASK;
	return x;
}

/*******************************
 * Absolute value.
 */

#undef fabs			/* disable macro version	*/
#undef fabsf

double fabs(double x)
{
	P(x)[3] &= ~ SIGNMASK;
	return x;
}


float fabsf(float x)
{
	P(x)[1] &= ~ SIGNMASK;
	return x;
}



/*******************************
 * Generate various types of NaN's.
 */

double __CDECL nan(const char *tagp)	{ return NAN; }

float __CDECL nanf(const char *tagp)	{ return NAN; }

double __CDECL nans(const char *tagp)	{ return NANS; }

float __CDECL nansf(const char *tagp)
{
    return NANS;
/*
    static long x = 0x7F810000;
    return *(float *)&x;
 */
}

#endif /* IEEE */
