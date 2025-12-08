;_ frexp.asm
; Written by Walter Bright
; Copyright (C) 1984-1991 by Walter Bright
; All rights reserved
; For the 386

	include	macros.asm

	.287

	begdata
	extrn	__8087:word
	enddata

	begcode	frexp

longexp	=	07FF00000h
longhid =	000100000h
longbias =	03FFh

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	#include <math.h>
;	double frexp(double value,int *eptr)
; Returns:
;	x such that value=x*2**n, .5 < |x| <= 1.0
;	x has same sign as value.
;	*eptr = n
;
;	Special cases:
;		value	  x	*eptr
;		+-0.0	+-0.0	  0
;		+-inf	+-inf	  *
;		+-NaN	+-NaN	  *
;		+-NaNs	+-NaN	  *
;
;	* Indeterminate
;	No exceptions are raised, even if value is a signaling NaN.

	c_public frexp

func	frexp
	push	EBP
	mov	EBP,ESP
  if 0	;Using the x87 is a problem because FXTRACT returns a value
	;between 1 and 2 instead of .5 to 1, also the different x87's
	;have different behavior on special values, and set the
	;exception flags strangely.

	_ifs	__8087 e 0, FR4		;if no 8087
	fld	qword ptr P[EBP]
	fxtract
	fstp	qword ptr P[EBP]
    if SPTR
	mov	EDX,P+8[EBP]
	fistp	dword ptr [EDX]		;store *eptr
    else ;LPTR
	les	EDX,P+8[EBP]
	fistp	dword ptr ES:[EDX]
    endif
	mov	EAX,P+0[EBP]
	fwait
    if SPTR
	_ifs	<dword ptr [EDX]> e 080000000h, FR5	;check for -infinity
    else
	_ifs	<dword ptr ES:[EDX]> e 080000000h, FR5	;check for -infinity
    endif
FR6:	mov	EDX,P+4[EBP]
	pop	EBP
	ret

	;The 80387 gives -infinity as a result if value is 0.0. To
	;conform to ANSI, set *eptr to 0. The 8087 and 80287 already yield 0.
FR5:
    if SPTR
	shl	dword ptr [EDX],1	;make it 0
    else
	shl	dword ptr ES:[EDX],1	;make it 0
    endif
	jmp	FR6
FR4:
  endif

	uses	<ECX,EDI>
	mov	EAX,P+0[EBP]
	mov	EDX,P+4[EBP]

	mov	EDI,EDX
	and	EDI,longexp
	jz	short FRzeroexp		;zero exponent
	_ifs	EDI e longexp, FR7	;if infinity or NaN
	shr	EDI,20			;right justify it
	sub	EDI,longbias		;un-bias it
FR8:	and	EDX,800FFFFFh		;scrap exponent bits
	mov	ECX,EDX
	and	ECX,000FFFFFh		;clear sign bit
	or	ECX,EAX			;see if 1
	mov	ECX,03FF00000h
	jz	short FR3		;yes
	inc	EDI
	mov	ECX,03FE00000h		;to get .5 < |x| <= 1
FR3:	or	EDX,ECX			;to get .5 < |x| <= 1
FR9:
    if LPTR
	les	ECX,P+8[EBP]		;ES:ECX = eptr
	mov	ES:[ECX],EDI
    else
	mov	ECX,P+8[EBP]		;ECX = eptr
	mov	[ECX],EDI
    endif

FR2:	unuse	<EDI,ECX>
FR0:	pop	EBP
	ret

FRzeroexp:
	;Determine if 0 or subnormal
	mov	EDI,EDX
	shl	EDI,1
	or	EDI,EAX			;is value 0?
	jz	short FR9		;yes (DI is 0)

	;It's subnormal
	;Left justify subnormal significand, decrementing exponent as we go
	mov	EDI,1-longbias
	mov	ECX,EDX
	and	ECX,80000000h		;save original sign
FRS1:	dec	EDI
	shl	EAX,1
	rcl	EDX,1
	test	EDX,longhid
	je	short FRS1
	or	EDX,ECX			;restore sign bit
	jmp	FR8

	;value is +-INFINITY, or a NAN
FR7:	;(don't worry about *exp in this case)
	mov	EDI,EDX
	and	EDI,0FFFFFh		;isolate significand bits
	or	EDI,EAX
	jz	short FR2		;it's +-INFINITY
	or	EDX,80000h		;convert NANS to NANQ
	jmp	short FR2

c_endp	frexp

	endcode	frexp

	end
