/*
 *	lib.b		bc math library
 *
 *	25 Apr 90	ACH
 */


scale=20


/*
 *	exp( x )	for all real x
 *
 *	1. Scale all values of |x| > 2 onto the range [-2, 2] by dividing by
 *	   powers of two (2^p) to get x'. Series evaluation is much faster for 
 *	   values of |x'| <= 2. 
 *
 *	2. Perform series evaluation.
 *
 *	          ~  x'^i             x'^2   x'^3
 *	e^x'  =  sum ----  =  1 + x' + ---- + ---- + ...  (all real x)
 *	         i=0  i!               2!     3!
 *
 *	3. Scale e^x up again: e^x = (e^x')^2^p
 */
define e( x ){
	auto a, b, c, i, p, s, t
	t = scale
	scale += .434*x + 4
	for( p = 0; x > 2; ++p ) x *= .5 
	while( x < -2 ){ x *= .5; ++p }
	a = x
	b = 1
	c = 1+x
	s = 0
	for( i = 2; c != s; ++i ){
		s = c
		a *= x 
		b *= i
		c += a/b
	}
	while( p-- ) s *= s
	scale = t
	return( s/1 )
}


/*
 *	ln( x )		all x > 0
 *
 *	1. Scale all values x onto the range [.5, 2]. Series evaluates quickly
 *	   on this range.
 *
 *	2. Series evaluation:
 * 
 *	                  x-1   1  x-1      1  x-1
 *	f*ln( x )  = f*2{ --- + - (---)^3 + - (---)^5 + ... }  (x > 0)
 *	                  x+1   3  x+1      5  x+1
 *
 */
define l( x ){
	auto a, b, c, f, i, s, t
	if( x <=0 ) return( 1-10^scale )
	t = scale
	scale += length( x ) - scale( x ) + 4
	for( f = 1; x > 2; f *= 2 ) x = sqrt( x );
	for( f; x <.5; f *= 2 ) x = sqrt( x );
	c = (x-1)/(x+1)
 	a = c
	b = c*c
	s = 0
	for( i = 3; c != s; i += 2 ){
		s = c
		a *= b
		c += a/i
	}
	scale = t
	return( 2*f*s/1 )
}


/*
 *	arctan( x )	for all real x
 *
 *	1. Scale all values of |x| > .5 onto the range [-.5, .5].
 *
 *	2. Perform series evauluation.
 *
 *			     x    x^3   x^5   x^7
 *	f*arctan( x ) = f { --- - --- + --- - --- +  ... }   (|x| < 1)
 *			     1     3     5     7
 *
 *	To increase the speed of functions that wish to estimate pi by 
 *	2*arctan(1), the value of arctan(1) to 200 decimal places is kept. 
 *	If the scale > 200 then perform the regular series evaluation.
 */
define a( x ){
	auto a, c, f, i, s, t
	if( x == 0 ) return( 0 )
	if( x == 1 ) if( scale < 100 ){
return( .785398163397448309615660845819875721049292349843776455243736148076954\
101571552249657008706335529266 / 1 )
	}
	t = scale
	scale += 3
	for( f = 1; .5 < x; f *= 2 ) x = (sqrt( 1 + x*x ) - 1)/x
	for( f; x < -.5; f *= 2 ) x = (sqrt( 1 + x*x ) - 1)/x
	a = x
	c = x
	x = -x*x
	s = 0
	for( i = 3; c != s; i += 2 ){
		s = c	
		a *= x
		c += a/i 
	}
	scale = t
	return( f*s/1 )
}


/*
 *	cos( x )	for all real x in radians
 *
 *		      ~		 x^2i	      x^2   x^4   x^6
 *	cos( x )  =  sum  (-1)^i ----  =  1 - --- + --- - --- + ... 
 *		     i=0         (2i)!         2!    4!    6!
 *
 *	or	cos( x )  =  sin( pi/2 - x )
 */
define c( x ){
	auto t
	t = scale
	scale += 1
	x = s( 2*a(1) - x )
	scale = t
	return( x/1 )
}


/*
 *	sin( x )	for all real x in radians
 *
 *	1. Reduce angle x to the range [-180, 180]. The series evalutes faster
 *	   for values in this range.
 *
 *	2. Perfom series evalutaion:
 *
 *		      ~		 x^2i+1	        x^3   x^5   x^7
 *	sin( x )  =  sum  (-1)^i ------  =  x - --- + --- - --- + ... 
 *		     i=0         (2i+1)!         3!    5!    7!
 */
define s( x ){
	auto a, b, c, i, s, t
	t = scale
	scale += 2 
	b = 2*a( 1 ) 
	scale = 0
	if( x >= 0 ) c = (x/b + 1)/2
	if( x < 0 ) c = (x/b - 1)/2
	x -= 2*c*b
	if( c%2 == 1 ) x = -x
	scale = scale( b ) 
	a = x
	b = 1
	c = x
	x = -x*x
	s = 0 
	for( i = 3; c != s; i += 2 ){
		s = c
		a *= x
		b *= i*i - i
		c += a/b
	}
	scale = t
	return( s/1 )
}


/*
 *	Bessel Functions of Interger Order
 *
 *	            x      ~     (-1)^k    x
 *	J(n,x)  =  (-)^n  sum  ---------- (-)^2k
 *	            2     i=0  i!(i + n)!  2
 *
 *	               x^2   x^4   x^6
 *	J(0,x)  =  1 - --- + --- - ---- + ....
 *                      4    64    2304
 *
 *	           x       x^2   x^4   x^6
 *	J(1,x)  =  - { 1 - --- + --- - ---- + ... }
 *	           2        8    192   9216
 */
define j( n, x ){
	auto a, b, c, i, j, s, t
	t = scale
	scale += 2
	if( n < 0 ){
		n = -n
		x = -x
	}
	a = 1 
	b = 1
	x *= .5
	for( j = 1; j <= n; ++j ){
		a *= x 
		b *= j
	}
	c = a/b 
	x = -x*x
	s = 0
	for( i = 1; c != s; ++i ){
		s = c
		a *= x
		b *= i*j
		c += a/b
		j += 1
	}
	scale = t
	return( c/1 )
}


/*	MKS bc has long name support */
define exp( x ) { return( e(x) ) }
define ln( x ) { return( l(x) ) }
define arctan( x ) { return( a(x) ) }
define cos( x ) { return( c(x) ) }
define sin( x ) { return( s(x) ) }
define bessel( n, x ) { return( j(n,x) ) }
