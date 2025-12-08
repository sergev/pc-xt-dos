;_double.asm   Mar 3 1991   Modified by: Phil Hinger */
;$Header$
;Copyright (C) 1984-1988 by Walter Bright
;All Rights Reserved, written by Walter Bright
;Floating point package

include macros.asm
include flthead.asm

	.8087

    ifdef _MT
    if LCODE
	c_extrn	_FEEXCEPT,far
	c_extrn	_FEROUND,far
    else
	c_extrn	_FEEXCEPT,near
	c_extrn	_FEROUND,near
    endif
    endif


	begdata
	c_extrn _8087,word		;8087 flag word

    ifndef _MT
	extrn	__fe_cur_env:word
    endif

	public	_DBL_MAX,_DBL_MIN,_FLT_MAX,_FLT_MIN


_DBL_MAX	dd	0,longexp	;maximum double value
_DBL_MIN	dd	0,longhid	;minimum
_FLT_MAX	dd	shortexp	;maximum float value
_FLT_MIN	dd	shorthid


	enddata

	begcode	double

	;Note:	0=int	2=unsigned	3=long	4=float	5=double

	public		exception, dunnorm, dround, dget_dtype
	public		dleft_justify,dnorm, dget_dtype_pair
	public		__DSUB@
	c_public	_FLTDBL@,_DBLFLT@
	c_public	_DADD@,_DMUL@,_DDIV@,	    _DTST@
	c_public	_DTST0@,_DTST0EXC@
	c_public	_DCMP@,_DCMPEXC@
	c_public	_DBLINT@,_INTDBL@,_DBLUNS@,_UNSDBL@
	c_public	_DBLLNG@,_LNGDBL@,_DBLULNG@,_ULNGDBL@
	c_public	__dtype

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Short real:
;	s | exponent| significand|
;	31|30	  23|22		0|
; Long real:
;	s | exponent| significand|
;	63|62	  52|51		0|


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Unpack a double. The double must not be 0.
; Input:
;	EDX,EAX = the double (with sign = 0)
; Output:
;	EDX,EAX = significand (with hidden bit in EDX bit 31)
;	SI     exponent
;	EDI	sign (in bit 31)

	_align
dunnorm proc	near
	mov	EDI,EDX		;save sign
	mov	ESI,EDX
	and	ESI,longexp	;mask off exponent bits
	jne	dunnorm1	;special case when exponent is zero
	call	dleft_justify
	jmps	dunnorm2
dunnorm1:
	shr	ESI,4+16	;right-justify exponent
	or	EDX,longhid	;or in hidden bit
dunnorm2:
	; EDX,EAX <<= 11
	shld	EDX,EAX,11
	shl	EAX,11
	ret
dunnorm endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Left justify mantissa when
; exponent is zero
;
;  Input:
; [EDX,EAX] = double
; SI		= Exponent
;
dleft_justify proc near
	mov	SI,1

dleft_justify1:
	dec	SI			;Adjust exponent
	shl64	EDX,EAX			;shift mantissa left
	test	EDX,longhid		;is it shifted enough
	je	dleft_justify1		;no
	ret
dleft_justify endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Round and normalize and add
;Input:
;	[EDX,EAX] = significand
;	assume bit 7 or 8 of AH is set
;	if bit 8 of AH is set then
;		 bits 1 and 2 of DH are sticky bits and bit
;		3 in DH is the guard bit.
;	else
;		bits 1 of DH is a sticky bits and bit
;	2 in AH is the guard bit.
;	AL = other sticky bits
;	SI = exponent (biased)
;	EDI (sign bit)
;
	_align
dround	proc	near
	dec	SI		;Make sure exponent is correct if not shifted
	test	EDX,sgn		;if MSB is set then
	jz	dround1		;adjust shift register and
	shr64	EDX,EAX
	adc	AL,0		;make sure sticky bit dit not drop off
	inc	SI		;adjust exponent

dround1:
	_ifs	 SI l 07FFh, dround11
	jmp	 dpackOverflow


dround11:
	_ifs SI g 0, dround7
	dec	SI	       ;adjustment so it will shift ok
	call	dright_justify	;shift right until SI is zero

dround7:
	test	AH,11b
	jne	dround6
	or	AL,AL
	je	dround5

dround6:
	tst	SI
	jne	dround10
	or	__fe_cur_env.status,FE_UNDERFLOW
dround10:
	or	__fe_cur_env.status,  FE_INEXACT	;no longer exact
	_ifs	 __fe_cur_env.round e  FE_TONEAREST, dround3
	_ifs	 __fe_cur_env.round e  FE_TOWARDZERO,dround5
	_ifs	 __fe_cur_env.round ne FE_DOWNWARD,dround2


	;FE_DOWNWARD
	test	EDI,sgn
	jns	dround5
	jmps	dround4


dround2:;FE_UPWARD
	test	EDI,sgn
	js	dround5
	jmps	dround4


dround3:;FE_TONEAREST
	test	AH,10b		;If guard bit is not set then
	jz	dround5		;no rounding is necessary
	test	AH,101b		;Test if odd or sticky bits are set
	jne	dround4
	tst	AL		;Test other sticky bits
	jz	dround5


dround4:;round up
	add	EAX,0400h
	adc	EDX,0
	tst	SI	       ;is exponent zero
	je	dround9		;yes

	test	EDX,sgn   ;is msb still where is should be
	jz	dround5
	inc	SI
	shr64	EDX,EAX

dround5:
	shrd	EAX,EDX,10
	shr	EDX,10
	jmp	dpack

dround9:			;when exponent is zero
	test	EDX,40000000h	;is msb still where is should be
	jz	dround5		;yes
	inc	SI	       ;exponent is now 1
	jmps	dround5
dround	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Normalize and pack a double.
; Input:
;	[EDX,EAX] = significand
;	SI = exponent (biased)
;	EDI bit 15 = sign of result
;
dnorm	proc	near
	sub	SI,11	       ;offset

dnorm2:
	test	EDX,0FFE00000h	;do we need to shift right?
	jz	dnorm3		;no

dnorm1:
	shr64	EDX,EAX
	inc	SI	       ;exponent
	jnc	dnorm2
	test	EDX,0FFE00000h	;done shifting yet?
	jnz	dnorm1


;see if our significand is 0
dnorm3:
	test	EDX,001F0000h
	jnz	dnorm4
	test	EDX,EDX
	jnz	dnorm5
	shld	EDX,EAX,16
	shl	EAX,16
	sub	SI,16		;shift left by 16
	tst	EDX
	jnz	dnorm5
	shld	EDX,EAX,16
	clr	EAX		;EDX,EAX <<= 16
	sub	SI,16
	tst	EDX
	jz	dpack2		;result is 0
	tst	DH
	jnz	dnorm5
	xchg	DH,DL		;EDX <<= 8
	sub	SI,8


dnorm4: test	EDX,longhid	 ;hidden bit in right spot?
	jnz	dnorm6		;no
dnorm5:
	shl64	EDX,EAX     ;shift left till it is
	dec	SI
	jmp	dnorm4

dnorm6:
	_ifs SI ge 0, dpack
	shld	EDX,EAX,10
	shl	EAX,10
	dec	SI		;adjustment so it will shift ok
	call	dright_justify	;shift right untell SI is zero
	jmp	dround7
dnorm	endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Pack a double.
; Input:
;	[EDX,EAX] = significand
;	SI = exponent (biased)
;	EDI bit 15 = sign of result
;
dpack	proc	near
	shl	ESI,4+16
	_ifs	 ESI ae 07FF00000h, dpackOverflow

	and	EDX,longhid - 1	;dump hidden bit
	or	EDX,ESI		;install exponent
	and	EDI,sgn		;mask sign bit
	or	EDX,EDI		;install sign
dpack2:	ret

dpackOverflow:
	or	__fe_cur_env.status,FE_OVERFLOW or FE_INEXACT
	mov	EAX,2		;overflow
	jmp	exception	;raise overflow exception
dpack	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Raise underflow/overflow exception
; Input:
;	EDI	bit 15 is sign bit of result
;	EAX	1 = underflow
;		2 = overflow
; Returns:
;	EDX,EDX	adjusted result

exception proc near
	clr	EDX
	dec	EAX
	jz	FPV3		;0 is result for underflow
	mov	EDX,longexp
	clr	EAX

	;Adjust infinity based on rounding mode
	;NEAREST	infinity with sign
	;DOWN		+ overflows to + largest finite, - overflows to -inf
	;UP		- overflows to - largest finite, + overflows to +inf
	;TOZERO		to signed largest finite

	_ifs	 __fe_cur_env.round e FE_TONEAREST, FPV3
	_ifs	 __fe_cur_env.round e FE_TOWARDZERO, FPV1
	_ifs	 __fe_cur_env.round e FE_UPWARD, FPV2

	tst	EDI
	js	FPV3
	jmps	FPV1

FPV2:	tst	EDI
	jns	FPV3

	;Generate largest finite
FPV1:	mov	EDX,07FEFFFFFh
	dec	EAX		;to 7FEF FFFF FFFF FFFF

FPV3:	and	EDI,sgn
	or	EDX,EDI		;install sign bit
	ret
exception endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; get index of a pair of double
;	index = (case(b)*dtype_base + case(c)) * 2
; input:
;	[ECX,EBX] = b
;	[EDX,EAX] = c
; Output:
;	SI = index
;
dget_dtype_pair proc near
	clr	ESI				;set to default
	mov	EDI,ECX
	test	EDI,7fffffffh
	jz	dget_dtype_pair1

	and	EDI,longexp				 ;mask exponent
	_ifs	EDI ne long_infinity, dget_dtype_pair2	;b is not NaN or infinite
	mov	EDI,ECX
	test	EDI,dqnan_bit				 ;bit must be set to be
	jz	dget_dtype_pair3			 ;a quit NaN
	mov	SI,dtype_qnan*4				 ;b is a quite NaN
	jmps	dget_dtype_pair2

dget_dtype_pair3:
	and	EDI,7ffffh
	or	EDI,EBX					 ;Mantissa must be zero to be infinite
	jz	dget_dtype_pair4			 ;otherwise
	mov	SI,dtype_snan*4				 ;b is a signaling NaN
	jmps	dget_dtype_pair2

dget_dtype_pair4:					;b is infinite
	mov	SI,dtype_infinite*4
	jmps	dget_dtype_pair2

dget_dtype_pair1:
	tst	EBX
	jnz	dget_dtype_pair2
	mov	SI,dtype_zero*4				;b is zero

dget_dtype_pair2:
	test	EDX,7fffffffh
	jz	dget_dtype_pair5

	mov	EDI,EDX
	and	EDI,longexp				;mask exponent
	_ifs	EDI ne long_infinity, dget_dtype_pair6 ;c is not NaN or infinite
	mov	EDI,EDX
	test	EDI,dqnan_bit				;bit must be set to be
	jz	dget_dtype_pair7			;a quit NaN
	add	SI,dtype_qnan * dtype_base*4		;c is a quite NaN
	ret

dget_dtype_pair7:
	and	EDI,7ffffh
	or	EDI,EAX					;Mantissa must be zero to be infinite
	jz	dget_dtype_pair8			;otherwise
	add	SI,dtype_snan * dtype_base*4		;c is a signaling NaN
	ret

dget_dtype_pair8:					;c is infinite
	add	SI,dtype_infinite * dtype_base*4
	ret

dget_dtype_pair5:
	tst	EAX
	jnz	dget_dtype_pair6
	add	SI,dtype_zero * dtype_base*4		;c is zero

dget_dtype_pair6:
	ret
dget_dtype_pair endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; get special case index of  double
;	index = case(a)
; input:
;	[EDX,EAX] = a
; Output:
;	SI = index
;
dget_dtype proc near
	clr	ESI				;set to default
	test	EDX,7fffffffh
	jz	dget_dtype1

	mov	EDI,EDX
	and	EDI,longexp				;mask exponent
	_ifs	 EDI ne long_infinity, dget_dtype2	 ;c is not NaN or infinite
	mov	EDI,EDX
	test	EDI,dqnan_bit				 ;bit must be set to be
	jz	dget_dtype3				;a quit NaN
	mov	SI,dtype_qnan					;c is a quite NaN
	ret

dget_dtype3:
	and	EDI,7ffffh
	or	EDI,EAX					;Mantissa must be zero to be infinite
	jz	dget_dtype4				;otherwise
	mov	SI,dtype_snan					;c is a signaling NaN
	ret

dget_dtype4:						;c is infinite
	mov	SI,dtype_infinite
	ret

dget_dtype1:
	tst	EAX
	jnz	dget_dtype2
		mov	SI,dtype_zero				;c is zero

dget_dtype2:
	ret
dget_dtype endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; classify double float
; input:
;	[P] = a
; Output:
;	AX = classification
;
	 b = P

	c_public __fpclassify_d

func	__fpclassify_d
	push	EBP
	mov	EBP,ESP
	_push	<ESI,EDI>
	mov	EDX,b+4[EBP]
	mov	EAX,b+[EBP]	       ;mov a into registers
	mov	ESI,FP_NORMAL	       ;set to default
	mov	EDI,EDX
	and	EDI,longexp	       ;mask exponent
	je	fpclassify_d1	       ;set if exponent is zero

	_ifs	EDI e long_infinity, fpclassify_d2 ;test for NaN or infinite

fpclassify_dDone:
	mov	EAX,ESI			;return classification
	_pop	<EDI,ESI>
	pop	EBP
	ret

fpclassify_d2:
	mov	SI,FP_NANQ		;assumes quiet NaN
	test	EDX,dqnan_bit		;bit must be set to be
	jnz	fpclassify_dDone	;a quiet NaN

	mov	SI,FP_INFINITE		;assume Infinity
	mov	EDI,EDX
	and	EDI,mantisa_mask	;clear sign and exponent
	or	EDI,EAX			;all ather bit must be zort to be inifite
	jz	fpclassify_dDone	;otherwise

	mov	SI,FP_NANS		;a is a signaling NaN
	jmps	fpclassify_dDone


fpclassify_d1:
	mov	SI,FP_ZERO		;assume Zero
	mov	EDI,EDX
	and	EDI,mantisa_mask	;drop sign and exponent
	or	EDI,EAX			;are any other bit set
	jz	fpclassify_dDone
					;if not then it must be subnormal
	mov	SI,FP_SUBNORMAL
	jmps	fpclassify_dDone
c_endp	__fpclassify_d


;Condition code values
CCeq	equ	40h
CClt	equ	81h
CCgt	equ	00h
CCinv	equ	45h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Put into DI the OR of all the significand bits

orsigc	proc	near
	mov	EDI,ECX
	and	EDI,mantisa_mask
	or	EDI,EBX
	ret
orsigc	endp

orsigb	proc	near
	mov	EDI,EDX
	and	EDI,mantisa_mask
	or	EDI,EAX
	ret
orsigb	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Test and see if [EDX,EAX] is 0

	_align
func	_DTST@
	push	EDX
	shl	EDX,1		;dump sign bit
	or		EDX,EAX
	pop	EDX
	ret
c_endp	_DTST@

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Double compare against 0, setting sign bits appropriately
;	a = b ? 0
; Input:
;	[EDX,EAX] = b
; Output:
;	[EDX,EAX] = b
; _DTST0EXC@ is same as _DTST0@, but set invalid exception flag if
; the operand is a NAN.

	_align
func	_DTST0EXC@
	stc
	jmp	short DT9
c_endp	_DTST0EXC@

	_align
func	_DTST0@
if 1
	clc
DT9:	push	ESI
	sbb	ESI,ESI			  ;SI==-1 if raise exception
	push	EDI
	push	EAX
	push	EDX

	;Test if b is a special value
	mov	EDI,EDX
	and	EDI,longexp
	jz	DT1			   ;c is 0 or subnormal
	_ifs	EDI e longexp, DT2	   ;c is nan or infinity

	;Do a straightforward comparison
DT3:	tst	EDX
	jz	DT8
	mov	AH,CCgt
	jg	DTret
	mov	AH,CClt
DTret:	;or	AX,AX			;OF (overflow flag) is already clear
	sahf
	_pop	<EDX,EAX,EDI,ESI>
	ret

DT8:	or	EDX,EAX
	jne	DT7

DTeq:	mov	AH,CCeq
	jmp	DTret

DT7:	mov	AH,CCgt
	ja	DTret
	mov	AH,CClt
	jmp	DTret

DT1:	call	orsigb
	jnz	DT3			;subnormal
	jmp	DTeq			;b is 0

DT2:	call	orsigb
	jz	DT3			;b is infinity

	;b is a NAN
	mov	AH,CCinv
	and	SI,FE_INVALID
	or	__fe_cur_env.status,SI
	jmp	DTret
else
	push	EDX
	tst	EDX
	js	TST01
	or	EDX,EAX
	neg	EDX
	sbb	EDX,EDX
	neg	EDX
	pop	EDX
	ret

TST01:
	shl	EDX,1
	or	EDX,EAX
	neg	EDX			;C=1 if b!=0
	sbb	EDX,EDX			;if (b==0) EDX=0 else EDX=-1
	pop	EDX
	ret

endif
c_endp	_DTST0@

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Double compare
;	a = b ? c
; Input:
;	EDX,EAX = b
;	ECX,EBX = c
; Output:
;	no registers changed
;	Condition codes set same as 8087 would
;	(but also set SF and OF so that pre-3.0 code will link which
;	used signed jmps after DCMP)
;
; _DCMPEXC@ is same as _DCMP@, but set invalid exception flag if
; either of the operands are NAN.

if 1
	_align
func	_DCMPEXC@
	stc
	jmp	short DC9
c_endp	_DCMPEXC@

	_align
func	_DCMP@
	clc
DC9:

	push	ESI
	sbb	ESI,ESI			  ;SI==-1 if raise exception
	push	EDI
	push	ECX
	push	EAX

	;Test if c is a special value
	mov	EDI,ECX
	and	EDI,longexp
	jz	DC1			 ;c is 0 or subnormal
	_ifs	 EDI e longexp, DC2	;c is nan or infinity

	;Test if b is a special value
DC3:	mov	EDI,EDX
	and	EDI,longexp
	jz	DC4			;b is 0 or subnormal
	_ifs	EDI e longexp, DC5     ;b is nan or infinity

	;Do a straightforward comparison
DC6:	mov	EDI,ECX
	xor	EDI,EDX
	js	DC8			;signs are different
	_ifs	EDX ne ECX, DC7
	_ifs	EAX ne EBX, DC7

DCeq:	mov	AH,CCeq
DCret:	or	AX,AX			;clear OF (overflow flag)
	sahf
	pop	EAX
	pop	ECX
	pop	EDI
	pop	ESI
	ret

DC7:
	mov	AH,CCgt
	ja	DC10
	mov	AH,CClt
DC10:	test	ECX,sgn
	jns	DCret
	xor	AH,CCgt XOR CClt
	jmp	DCret

DC8:	test	ECX,sgn
	mov	AH,CClt
	jns	DCret
	mov	AH,CCgt
	jmp	DCret

DC1:	call	orsigc
	jnz	DC3			;subnormal
	;c is +0 or -0
	and	ECX,sgn_mask		;no -0 bugs
	jmp	DC3			;c is 0

DC2:	call	orsigc
	jz	DC3			;c is infinity
	jmp	short DCinv		;c is a nan

DC4:	call	orsigb
	jnz	DC6			;b is subnormal
	;c is +0 or -0
	and	EDX,sgn_mask		 ;no -0 bugs
	jmp	DC6			;b is 0

DC5:	call	orsigb
	jz	DC6			;b is infinity
;	jmp	DCinv			;b is a nan

DCinv:	mov	AH,CCinv
	and	SI,FE_INVALID
	or	__fe_cur_env.status,SI
	jmp	DCret
c_endp	_DCMP@

else
	_align
func	_DCMP@
	push	EDI
	push	EDX
	push	ECX

	;test if c is 0
	mov	EDI,ECX
	shl	EDI,1			;dump sign bit
	or		EDI,EBX
	jnz	C3			;no
	and	ECX,7F000000h		;no -0 bugs
C3:
	mov	EDI,EDX
	shl	EDI,1			;dump sign bit
	or	EDI,EAX
	jnz	C2			;no
	and	EDX,7F000000h		;convert -0 to 0
C2:
	mov	EDI,EDX
	xor	EDI,ECX
	js	C52			;signs are different
	mov	EDI,1			;1 for positive compares
	tst	ECX
	jns	C51
	neg	EDI			;-1 for negative compares
C51:	_ifs	EDX ne ECX, C6		;compare MSW
	_ifs	EAX e EBX, L21		;compare LSW
C6:	ja	C7
	neg	EDI
C7:	tst	EDI
L21:	pop	ECX
	pop	EDX
	pop	EDI
	ret

C52:	cmp	EDX,ECX
	jmp	L21
c_endp	_DCMP@
endif



;;;;
; Right justifty mantissa when
; exponent is less then zero
;
;  Input:
;  [AX,BX,CX,DX] = double
;  SI		 = Exponent
;
dright_justify proc near
	_ifs SI l -56, dright_justify5

dright_justify1:
	_ifs	 SI g -8, dright_justify2
	sh64StyRBy8
	add	SI,8
	jnz	dright_justify1

dright_justify2:
	xchg	ECX,ESI
	and	ECX,0ffffh	;make sure top is clear
	jcxz	dright_justify3	;no shifting need be done
	neg	CX
	or	AL,AL		;If any sticky bits are set then
	je	dright_justify4	;make sure they don't all rotate out
	or	AL,40h

dright_justify4:
	shr64	EDX,EAX
	loop	dright_justify4

dright_justify3:
	xchg	ECX,ESI		  ;restore CX, SI = 0
	ret

dright_justify5:
	or	EAX,EDX
	je	dright_justify6
	clr	EDX
	mov	EAX,1

dright_justify6:
	mov	SI,DX
	ret
dright_justify endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Double floating add/subtract.
;	a = b +(-) c
; Input:
;	[ECX,EBX]  = b
;	[EDX,EAX]  = c
; Output:
;	a = [EDX,EAX]
;	SI,DI = preserved
; Stack offsets


	padnn	 =	24	;so nn == dd == mm == 50
	sign	 =	padnn+8
	signc	 =	sign+4
	subtract =	signc+4
	exp	 =	subtract+4
	orgsign  =	exp+2
	nn	 =	orgsign+4
	b	 =	nn+P




__DSUB@:
	push	EBP
	sub	ESP,nn		 ;make room for nn variables
	mov	EBP,ESP
	_ifs	 __8087 e 0, A6		  ;if no 8087
	mov	0[EBP],EAX
	mov	4[EBP],EDX
	fld	qword ptr 0[EBP]	;load b
	_push	<ECX,EBX>		;push c
	fsub	qword ptr -8[EBP]	;add c
	jmps	fltret

A6:
	mov	dword ptr orgsign[EBP],sgn ;need to flip sign back on  NaNs
	xor	ECX,sgn			 ;flip sign for subtraction
	jmps	A1

	_align
func	_DADD@
	push	EBP
	sub	ESP,nn			 ;make room for nn variables
	mov	EBP,ESP
	mov	dword ptr orgsign[EBP],0 ;need for NaN
	_ifs	 __8087 e 0, A1		  ;if no 8087
	mov	0[EBP],EAX
	mov	4[EBP],EDX
	fld	qword ptr 0[EBP]	;load b
	_push	<ECX,EBX>		;push c
	fadd	qword ptr -8[EBP]	;add c

fltret:
	;Check for floating point error
	fstsw	-2[EBP]
	fwait
	mov	AX,-2[EBP]
	and	AX,FE_ALL_EXCEPT
	jnz	fltret5			;jmp if error
fltret4:
	fstp	qword ptr -8[EBP]
	fwait				;wait for finish
	_pop	<EAX,EDX>		;pop results
	add	ESP,nn
	pop	EBP
	ret

fltret5:
	or	__fe_cur_env.status,AX
	jmp	fltret4


A1:
	push	EDI
	push	ESI
	call	dget_dtype_pair
	jmp	dword ptr cs:Daddindex[ESI]

Daddindex	label	dword
	dd	ANormalAdd		;other + other
	dd	AFirstIsAnswer		;other + zero
	dd	ASecondIsAnswer		;other + infinite
	dd	ASecondAsQNaN		;other + SNaN
	dd	ASecondQNaN		;other + QNaN

	dd	ASecondIsAnswer		;zero + other
	dd	Azeros			;zero + zero
	dd	ASecondIsAnswer		;zero + infinite
	dd	ASecondAsQNaN		;zero + SNaN
	dd	ASecondQNaN		;zero + QNaN

	dd	AFirstIsAnswer		;infinite + other
	dd	AFirstIsAnswer		;infinite + zero
	dd	AInfiniteInfinite	;infinite + infinite
	dd	ASecondAsQNaN		;infinite + SNaN
	dd	ASecondQNaN		;infinite + QNaN

	dd	AFirstAsQNaN	       ;SNaN + other
	dd	AFirstAsQNaN		;SNaN + zero
	dd	AFirstAsQNaN		;SNaN + infinite
	dd	ALargestSNaNasQNaN	;SNaN + SNaN
	dd	ASecondAsQNaN		;SNaN + QNaN

	dd	AFirstQNaN		;QNaN + other
	dd	AFirstQNaN		;QNaN + zero
	dd	AFirstQNaN		;QNaN + infinite
	dd	AFirstAsQNaN		;QNaN + SNaN
	dd	ALargestQNaN		;QNaN + QNaN

ANormalAdd:

	mov	signc[EBP],EDX	  ;Save sign
	mov	EDI,ECX
	mov	sign[EBP],EDI
	xor	EDI,EDX		;if sign(b) != sign(c), then subtraction
	mov	subtract[EBP],EDI ;flag for subtraction

	call	dunnorm		;unpack second operand (c)
	mov	exp[EBP],SI	;save exponent of c
	xChgReg64		  ;exchange regs
	call	dunnorm		;unpack first operand (b)
	sub	SI,exp[EBP]	;SI = exp(b) - exp(c)
	jle	A2		;exp(b) > exp(c)
	add	exp[EBP],SI	;exponent is b
	xChgReg64
	neg	SI
	mov	EDI,signc[EBP]
	mov	sign[EBP],EDI		  ;reset sign
A2:
	call	dright_justify
	test	dword ptr subtract[EBP],sgn ;subtracting? (test bit 15)
	je	A3			;no

	sub	EAX,EBX
	sbb	EDX,ECX

	jnc	A4			;no borrow
	xor	dword ptr sign[EBP],sgn  ;toggle sign of result
	neg64				;SI must be 0 for this to work

A4:
	test	EDX,0C0000000h		;if bit 8 or 7 are set then
	jnz	A5				;do normal round
	mov	EDI,EDX
	or	EDI,EAX			;is result zero
	je	Azeros1			;yes

	mov	EDI,sign[EBP]
	mov	SI,exp[EBP]		;exponent of result
	call	dnorm			;normalize and pack
	jmp	ADone

A3:
	add	EAX,EBX
	adc	EDX,ECX
	jnc	A5
	rcr	EDX,1
	rcr	EAX,1
	inc	word ptr exp[EBP]      ;bump exponent

A5:
	mov	EDI,sign[EBP]
	mov	SI,exp[EBP]	       ;exponent of result
	call	dround		       ;round and normalize
	jmp	ADone


Azeros:
	mov	EDI,EDX
	xor	EDI,ECX
	test	EDI,sgn			;are signs the same
	jne	Azeros1
	jmp	ADone			;yes

Azeros1:
	clr	EDX
	_ifs	 __fe_cur_env.round e FE_DOWNWARD,Azeros2
	jmp	AFirstIsAnswer
Azeros2:
	mov	EDX,sgn		       ;set sign to -
	jmps	AFirstIsAnswer
	
AInfiniteInfinite:
	mov	EDI,EDX
	xor	EDI,ECX
	test	EDI,sgn			;are signs the same
	je	AFirstIsAnswer		;yes

					;Default invalid operation
	mov	EDX,long_qnan OR 2000h
	clr	EAX
	or	__fe_cur_env.status,FE_INVALID
	jmps	ADone


ASecondAsQNaN:
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID
	jmps	ASecondQNaN


ALargestSNaNasQNaN:
	or	EDX,dqnan_bit
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

ALargestQNaN:
	xor	ECX,orgsign[EBP]		  ;reset orginal sign
	mov	ESI,EDX
	and	ESI,sgn_mask
	mov	EDI,ECX
	and	EDI,sgn_mask
	_ifs	ESI a EDI, AFirstIsAnswer
	jb	ASecondQNaN
	_ifs	EAX ae EBX, AFirstIsAnswer

ASecondQNaN:
	
	xor	ECX,orgsign[EBP]	  ;reset orginal sign
ASecondIsAnswer:
	mov	EDX,ECX
	mov	EAX,EBX
	jmps	ADone

AFirstAsQNaN:
	or	EDX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

AFirstQNaN:

AFirstIsAnswer:

ADone:
	pop	ESI
	pop	EDI
	add	ESP,nn
	pop	EBP
	ret
c_endp	_DADD@


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Double floating divide.
;	a = b / c
; Input:
;	[ECX,EBX] = b
;	[EDX,EAX] = c
; Output:
;	a = [EAX,EAX]
;	ESI,EDI preserved

; Stack offsets:

	paddd	=	24		;so nn == cxdd == mm == 50
	resp	=	paddd+16	;pointer to result
	sign	=	resp+2		;sign of result
	exp	=	sign+4		;exponent of result
	count	=	exp+2		;loop counter
	cxdd	=	count+2		;amount of local variables

func	_DDIV@
	push	EBP
	sub	ESP,cxdd
	mov	EBP,ESP

	_ifs	 __8087 e 0, D7		  ;if no 8087
	mov	0[EBP],EAX
	mov	4[EBP],EDX
	fld	qword ptr 0[EBP]	;load b
	_push	<ECX,EBX>		;push c
	fdiv	qword ptr -8[EBP]	;div c
	jmp	fltret

D7:	push	ESI
	push	EDI

	mov	sign[EBP],EDX			  ;transfer sig(b) to 0[EBP]
	xor	sign[EBP],ECX
	and	dword ptr sign[EBP],sgn
	call	dget_dtype_pair
	jmp	dword ptr cs:Dindex[ESI]

	_align
Dindex	label	dword
	dd	DNormalDivide		;other / other
	dd	DDivideByZero		;other / zero
	dd	DSignedZero		;other / infinite
	dd	DSecondAsQNaN		;other / SNaN
	dd	DSecondQNaN		;other / QNaN

	dd	DSignedZero		;zero / other
	dd	DDefaultQNaN		;zero / zero
	dd	DSignedZero		;zero / infinite
	dd	DSecondAsQNaN		;zero / SNaN
	dd	DSecondQNaN		;zero / QNaN

	dd	DSignedInfinite		;infinite / other
	dd	DSignedInfinite		;infinite / zero
	dd	DDefaultQNaN		;infinite / infinite
	dd	DSecondAsQNaN		;infinite / SNaN
	dd	DSecondQNaN		;infinite / QNaN

	dd	DFirstAsQNaN		;SNaN / other
	dd	DFirstAsQNaN		;SNaN / zero
	dd	DFirstAsQNaN		;SNaN / infinite
	dd	DLargestSNaNasQNaN	;SNaN / SNaN
	dd	DSecondAsQNaN		;SNaN / QNaN

	dd	DFirstQNaN		;QNaN / other
	dd	DFirstQNaN		;QNaN / zero
	dd	DFirstQNaN		;QNaN / infinite
	dd	DFirstAsQNaN		;QNaN / SNaN
	dd	DLargestQNaN		;QNaN / QNaN


DNormalDivide:
	;unpack c
	mov	EDI,ECX
	and	EDI,longexp		 ;mask off exponent bits
	and	ECX,0fffffh		;remove exponent from mantissa
	tst	EDI			;is exponent zero
	jnz	D12
					;special case when exponent is zero
	xChgReg64			  ;may need to do a lot of shifting
	call	dleft_justify		;msb must be block to left
	xChgReg64
	mov	DI,SI		;save exponent
	jmps	D13
D12:
	or	ECX,longhid
	shr	EDI,16+4
D13:
	
	;unpack b
	mov	ESI,EDX
	and	ESI,longexp		;mask off exponent bits
	and	EDX,0fffffh		;remove exponent from mantissa
	tst	ESI			;is exponent zero
	jnz	D14
					;special case when exponent is zero
	call	dleft_justify		;msb must be block to left
	jmps	D15
D14:
	or	EDX,longhid	       ;or in hidden bit
	shr	ESI,16 + 4
D15:
	sub	SI,DI			;exp(result) = exp(b) - exp(c)
	add	SI,longbias		;so bias is retained after subtraction
	mov	exp[EBP],SI		;exponent of result

	mov	ESI,ECX			;free up CX for loop counter
;;;;;;;
	;if (b >= c) goto D31 else D41
	mov	ECX,55			 ;16 bits per word
	mov	DI,1			;used to count to 16
	
	_align
D51:	_ifs	EDX a ESI, D31
	jb	D41
	_ifs	EAX b EBX, D41

	;b -= c

D31:	sub	EAX,EBX
	sbb	EDX,ESI			;since b > c, C == 0

D41:	rcl	EDI,1			;0 if we subtracted, 1 if not
	jc	D71			;push value every 16 loops

D61:	shl64	EDX,EAX			  ;b <<= 1
	loop	D51

	or	EAX,EDX
	je	D63
	mov	AL,1
D63:
	and	EAX,1
	mov	EDX,EDI
	not	EDX
	shl	EDX,9			 ;shift out count
	or	EAX,EDX
;;;;;;;

	pop	EDX			 ;load sig(result)
	mov	SI,exp[EBP]
	mov	EDI,sign[EBP]
	call	dround			;round and normalize result
	jmp	DDone


D71:	not	EDI			 ;push next mantissa on stack
	push	EDI
	clr	EDI
	jmps	D61

DDivideByZero:
	or	__fe_cur_env.status,FE_DIVBYZERO

DSignedInfinite:
	mov	EDX,sign[EBP]
	or	EDX,long_infinity
	clr	EAX
	jmps	DDone

DSignedZero:
	mov	EDX,sign[EBP]
	clr	EAX
	jmps	DDone

DSecondAsQNaN:
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

DSecondQNaN:
	xChgReg64
	jmps	DDone

	_align
DDefaultQNaN:
	mov	EDX,long_qnan OR 2000h
	or	EDX,sign[EBP]
	or	__fe_cur_env.status,FE_INVALID
	jmps	DDone


DLargestSNaNasQNaN:
	or	EDX,dqnan_bit
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

DLargestQNaN:
	mov	 ESI,EDX
	and	 ESI,sgn_mask
	mov	 EDI,ECX
	and	 EDI,sgn_mask
	_ifs	 ESI a EDI, DFirstQNaN
	jb	 DSecondQNaN
	_ifs	 EAX ae EBX, DFirstQNaN
	jmps	 DSecondQNaN

DFirstAsQNaN:
	or	EDX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID
DFirstQNaN:

DDone:
	pop	EDI
	pop	ESI
	add	ESP,cxdd
	pop	EBP
	ret
c_endp	_DDIV@


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Double floating multiply.
;	a = b * c
; Input:
;	[EBP] = b
;	[EDX,EAX] = c
; Output:
;	a = [EDX,EAX]
;	SI,DI preserved

; Stack offsets:

	padmm	=	32	;so nn == dd == mm == 50
	sign	=	padmm+8		;sig(b) + sig(c) + sig(result)
	exp	=	sign+4
	count	=	exp+4
	mm	=	count+2
	 b	=	mm + P

	_align
func	_DMUL@
	push	EBP
	sub	ESP,mm
	mov	EBP,ESP
	_ifs	__8087 e 0, M1		;if no 8087

	mov	0[EBP],EAX
	mov	4[EBP],EDX
	fld	qword ptr 0[EBP]	;load b
	_push	<ECX,EBX>		;push c
	fmul	qword ptr -8[EBP]	;mul c
	jmp	fltret

M1:	push	ESI
	push	EDI

	mov	sign[EBP],EDX			  ;transfer sig(b) to 0[EBP]
	xor	sign[EBP],ECX
	and	dword ptr sign[EBP],sgn
	call	dget_dtype_pair
	jmp	dword ptr cs:Mindex[ESI]

	_align
Mindex	label	dword
	dd	MNormalMultiply		;other * other
	dd	MSignedZero		;other * zero
	dd	MSignedInfinite		;other * infinite
	dd	MSecondAsQNaN		;other * SNaN
	dd	MSecondQNaN		;other * QNaN

	dd	MSignedZero		;zero * other
	dd	MSignedZero		;zero * zero
	dd	MDefaultQNaN		;zero * infinite
	dd	MSecondAsQNaN		;zero * SNaN
	dd	MSecondQNaN		;zero * QNaN

	dd	MSignedInfinite		;infinite * other
	dd	MDefaultQNaN		;infinite * zero
	dd	MSignedInfinite		;infinite * infinite
	dd	MSecondAsQNaN		;infinite * SNaN
	dd	MSecondQNaN		;infinite * QNaN

	dd	MFirstAsQNaN		;SNaN * other
	dd	MFirstAsQNaN		;SNaN * zero
	dd	MFirstAsQNaN		;SNaN * infinite
	dd	MLargestSNaNasQNaN	;SNaN * SNaN
	dd	MSecondAsQNaN		;SNaN * QNaN

	dd	MFirstQNaN		;QNaN * other
	dd	MFirstQNaN		;QNaN * zero
	dd	MFirstQNaN		;QNaN * infinite
	dd	MFirstAsQNaN		;QNaN * SNaN
	dd	MLargestQNaN		;QNaN * QNaN


MNormalMultiply:
	call	dunnorm		;unpack second operand (c)
	mov	exp[EBP],SI	;save exponent of c
	xChgReg64		  ;may need to do a lot of shifting
	call	dunnorm		;unpack second operand (b)
	sub	SI,longbias - 1 ;so bias is retained after add
	add	exp[EBP],SI	;exponent of result

	mov	ESI,EDX		; save b is [ESI,EDI]
	mov	EDI,EAX

				;b_low * c_low
	mul	EBX
	mov	[EBP],EAX
	mov	4[EBP],EDX

				;b_high * c_low
	mov	EAX,ESI
	mul	EBX
	add	4[EBP],EAX
	adc	EDX,0
	mov	EBX,EDX		  ;BX now free
	
				;b_low * c_high
	mov	EAX,EDI
	mul	ECX
	clr	EDI		 ;DI now free
	add	4[EBP],EAX
	adc	EBX,EDX
	adc	EDI,0		 ;save overflow bit

				;b_high * c_high
	mov	EAX,ESI
	mul	ECX
	add	EAX,EBX
	adc	EDX,EDI

	mov	ECX,[EBP]
	or	ECX,4[EBP]
	je	M2
	or	EAX,1		 ;save sticky bits
M2:
	mov	SI,exp[EBP]
	mov	EDI,sign[EBP]
	call	dround		;round and normalize result
	jmp	MDone

MSignedInfinite:
	mov	EDX,sign[EBP]
	or	EDX,long_infinity
	clr	EAX
	jmps	MDone

MSignedZero:
	mov	EDX,sign[EBP]
	clr	EAX
	jmps	MDone

MSecondAsQNaN:
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

MSecondQNaN:
	xChgReg64
	jmps	MDone

	_align
MDefaultQNaN:
	mov	EDX,long_qnan
	or	EDX,sign[EBP]
	or	__fe_cur_env.status,FE_INVALID
	jmps	MDone


MLargestSNaNasQNaN:
	or	EDX,dqnan_bit
	or	ECX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID

MLargestQNaN:
	mov	ESI,EDX
	and	ESI,sgn_mask
	mov	EDI,ECX
	and	EDI,sgn_mask
	_ifs	ESI a EDI, MFirstQNaN
	jb	MSecondQNaN
	_ifs	EAX ae EBX,MFirstQNaN
	jmps	MSecondQNaN

MFirstAsQNaN:
	or	EDX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID
MFirstQNaN:

MDone:
	pop	EDI
	pop	ESI
	add	ESP,mm
	pop	EBP
	ret

c_endp	_DMUL@
;;;;;;;;;;;;;;;;;;;;;;;;;;
	b	 =	P

func	__dtype
	push	EBP
	mov	EBP,ESP
	_push	<ESI,EDI,EBX>
	mov	EDX,b+4[EBP]
	mov	EAX,b[EBP]		 ;mov b into registers
	call	dget_dtype
	mov	EAX,ESI
	_pop	<EBX,EDI,ESI>
	pop	EBP
	ret
c_endp	__dtype


;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert unsigned short to double.
; ESI,EDI preserved.

func	_UNSDBL@
	clc
	jmps	INTDBL2
c_endp	_UNSDBL@

; Convert short to double

func	_INTDBL@
	_ifs	__8087 e 0, INTDBL1	;if no 8087
	sub	ESP,4			;2 extra words
	push	EAX
	fild	word ptr [ESP]		;load integer into 8087
INTDBL3:
	fstp	qword ptr [ESP]
	fwait			;wait for it to finish
	_pop	<EAX,EDX>	;pop result
	ret

INTDBL1:
	or	AX,AX			;negative? (also clear C)
	jns	INTDBL2			;no
	neg	AX			;abs value (also set C)
INTDBL2:
	_push	<ESI,EDI>
	rcr	EDI,1			;bit 15 becomes sign of result
	mov	EDX,EAX
	shl	EDX,16
	clr	EAX
	mov	ESI,15+longbias		;2^15
	call	dnorm			;pack result into a double
	_pop	<EDI,ESI>
	ret
c_endp	_INTDBL@

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert unsigned long to double.
; SI,DI preserved.

func	_ULNGDBL@
	clc
	jmps	A8
c_endp	_ULNGDBL@

; Convert long to double.

func	_LNGDBL@
	_ifs	__8087 e 0, A12		;if no 8087
	sub	ESP,4			;2 extra words
	push	EAX
	fild	dword ptr [ESP]		;load long into 8087
	jmp	INTDBL3

A12:	or	EAX,EAX			;negative? (also clear C)
	jns	A8			;no
	neg	EAX			;abs value
	stc				;indicate negative result
A8:	_push	<ESI,EDI>
	rcr	EDI,1			;bit 15 becomes sign of result
	mov	EDX,EAX
	clr	EAX			;rest of significand is 0
	mov	ESI,31+longbias        ;2^15
	call	dnorm			;pack result into a double
	_pop	<EDI,ESI>
	ret
c_endp	_LNGDBL@

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert double to unsigned.
      MinBitShift =  0
       stackSize  =  MinBitShift + 4


	_align
func	_DBLUNS@
	push	EBP
	sub	ESP,stackSize
	mov	EBP,ESP
	mov	word ptr MinBitShift[EBP],15
	test	EDX,sgn
	jz	dblIntgl

	or	__fe_cur_env.status,FE_INVALID
	add	ESP,stackSize
	pop	EBP
	ret
c_endp	_DBLUNS@

; Convert double to int.

func	_DBLINT@
	push	EBP
	sub	ESP,stackSize
	mov	EBP,ESP
	mov	word ptr MinBitShift[EBP],14
	test	EDX,sgn
	je	dblIntgl

	inc	word ptr MinBitShift[EBP]
	jmps	dblIntgl
c_endp	_DBLINT@


; Convert double to unsigned long.

func	_DBLULNG@
	push	EBP
	sub	ESP,stackSize
	mov	EBP,ESP
	mov	word ptr MinBitShift[EBP],31
	test	EDX,sgn
	jz	dblIntgl

	or	__fe_cur_env.status,FE_INVALID
	add	ESP,stackSize
	pop	EBP
	ret
c_endp	_DBLULNG@

; Convert double to long

func	_DBLLNG@
	push	EBP
	sub	ESP,stackSize
	mov	EBP,ESP
	mov	word ptr MinBitShift[EBP],30
	test	EDX,sgn
	jz	dblIntgl

	inc	word ptr MinBitShift[EBP]

dblIntgl:
	_push	<ESI,EDI,EBX,ECX>
	call	dget_dtype
	shl	ESI,2
	jmp	dword ptr cs:dblIntglIndex[ESI]

dblIntglIndex label	  word
	dd	dblIntglNormal		  ;other
	dd	dblIntglZero		  ;zero
	dd	dblIntglInvalid		  ;infinite
	dd	dblIntglInvalid		  ;SNaN
	dd	dblIntglInvalid		  ;QNaN

dblIntglNormal:
	call	dunnorm		    ;unpack double
	clr	EBX
	sub	SI,longbias	   ;un-bias the exponent
	js	dblIntgl4	    ;for neg exponents, the result is 0
	_ifs	SI a MinBitShift[EBP], dblIntglInvalid
	shld	EBX,EAX,8	    ;capture sticky bit and guard bits
	or	AH,AL
	or	BL,AH
	mov	EAX,EDX
	mov	ECX,31
	sub	CX,SI
	jcxz	dblIntgl2

	_align
dblIntgl3:
	shr	EAX,1
	rcr	BH,1		    ;keep stick bit
	adc	BL,0		    ;keep gaurd bit
	loop	dblIntgl3

dblIntgl2:
	mov	ESI,MinBitShift[EBP]
	tst	BX
	je	dblIntgl6
	or	__fe_cur_env.status,FE_INEXACT	;no longer exact
dblIntgl6:
	test	EDI,sgn		    ;is result negative?
	js	dblIntgl8
	jmps	dblIntglDone	    ;no


dblIntgl4:
	mov	BL,1		;save stick bits
	cmp	SI,-1		;is guard bit needed
	je	dblIntgl5
	or	BH,80h		;make guard bit into sticky bit
dblIntgl5:
	clr	EAX
	jmps	dblIntgl2
dblIntglInexact:
	or	__fe_cur_env.status,FE_INEXACT	;no longer exact
dblIntglZero:
	clr	EAX		 ;result is 0
	jmps   dblIntglDone

dblIntglInvalid:
	or	__fe_cur_env.status,FE_INVALID

dblIntglDone:
	_pop	<ECX,EBX,EDI,ESI>
	add	ESP,stackSize
	pop	EBP
	ret

dblIntgl8:
	mov	EDI,80000000h
	mov	SI,MinBitShift[EBP]
	_ifs	SI e 31, dblIntgl7
	
	mov	EDI,8000h
	

dblIntgl7:
	cmp	EAX,EDI
	je		dblIntglDone
	ja	dblIntglInvalid

	neg	EAX		    ;yes
	jmps	dblIntglDone
c_endp	_DBLLNG@
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; float
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Unpack a float. The float must not be 0.
; Input:
;	[EAX] = the float (with sign = 0)
; Output:
;	[EAX] = significand (with hidden bit in DX bit 15)
;	SI	exponent
;	EDI	sign (in bit 15)

public	funnorm
funnorm proc	near
	mov	EDI,EAX		  ;save sign
	test	EDI,shortexp
	jne	funnorm1
	clr	ESI
	call	fleft_justify
	jmps	funnorm2

funnorm1:
	mov	ESI,EAX
	and	ESI,sgn_mask
	shr	ESI,16+7
	or	EAX,shorthid	 ;or in hidden bit (80h)

funnorm2:
	shl	EAX,8
	ret
funnorm endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Normalize and pack a float.
; Input:
;	[EAX] = significand
;	SI = exponent (biased)
;	EDI bit 31 = sign of result
;
public	fnorm
fnorm	proc	near
	sub	SI,8		;offset

fnorm2:
	test	EAX,7f000000h	;do we need to shift right?
	jz	fnorm3		;no

fnorm1:
	shr	EAX,1
	inc	SI		;exponent
	jnc	fnorm2		;no rounding
	test	EAX,7f000000h	;done shifting?
	jnz	fnorm1		;no


;see if our significand is 0
fnorm3:
	tst	EAX
	jnz	fnorm4
	mov	EAX,shorthid
	clr	SI		;trick fnorm4 into giving us a 0 result

fnorm4:
	test	EAX,shorthid	 ;hidden bit in right spot?
	jnz	fnorm5		;yes
	shl	EAX,1		 ;shift left till it is
	dec	SI
	jmp	fnorm4

fnorm5:
	_ifs SI ge 0, fpack
	shl	EAX,7
	dec	SI
	call	fright_justify
	jmp	fround7
fnorm	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Pack a float.
; Input:
;	[EAX] = significand
;	SI = exponent (biased)
;	EDI bit 31 = sign of result
;

public	fpack
fpack	proc	near
	shl	ESI,16+7
	_ifs	 ESI ae short_infinity, fpackOverflow

	and	EAX,shorthid - 1 ;dump hidden bit
	or	EAX,ESI		;install exponent
	and	EDI,sgn		;mask sign bit
	or	EAX,EDI		 ;install sign
fpack2:	ret

fpackOverflow:
	or	__fe_cur_env.status,FE_OVERFLOW or FE_INEXACT
	mov	AX,2		;overflow
	jmp	fexception	;raise overflow exception
fpack	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Left justify mantissa when
; exponent is zero
;
;  Input:
; [EAX] = float
; SI		= Exponent
;
public fleft_justify

fleft_justify proc near
	mov	ESI,1

fleft_justify1:
	dec	SI			;Adjust exponent
	shl	EAX,1			;shift mantissa left
	test	EAX,shorthid		;is it shifted enough
	je	fleft_justify1		;no
	ret
fleft_justify endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Right justifty mantissa when
; exponent is less then zero
;
;  Input:
;  [EAX]  = float
;  SI	  = Exponent
;
public fright_justify

fright_justify proc near
	_ifs SI l -24, fright_justify5

fright_justify1:
	_ifs	 SI g -8, fright_justify2
	sh32StyRBy8
	add	SI,8
	jnz	fright_justify1

fright_justify2:
	xchg	ECX,ESI
	and	ECX,0ffffh	;make sure CX is clear
	jcxz	fright_justify3	;no shifting need be done
	neg	CX
	or	AL,AL		;If any sticky bits are set then
	je	fright_justify4	;make sure they don't all rotate out
	or	AL,40h

fright_justify4:
	shr	EAX,1
	loop	fright_justify4

fright_justify3:
	xchg	ECX,ESI		;restore CX, SI = 0
	ret

fright_justify5:
	tst	EAX
	je	fright_justify6
	mov	EAX,1
	jmps	fright_justify7

fright_justify6:
	clr	EAX
	
fright_justify7:
	clr	ESI
	ret
fright_justify endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Round and normalize and add
;Input:
;	[EAX] = significand
;	assume bit 30 or 31 of EAX is set
;	if bit 31 is set then
;		 all of AL make up the sticky bits and bit
;		1 in AH is the guard bit.
;	else
;		bit 7 of AL is the guard bit, the rest of the
;		bits int AL make up the sticky bits and bit
;	SI = exponent (biased)
;	DI (sign bit)
;
public	fround
fround	proc	near
	dec	SI		;Make sure exponent is correct if not shifted
	test	EAX,sgn		;if bit 8 of AH is set then
	jz	fround1		;adjust shift register and
	shr	EAX,1
	jnc	fround12
	or	AL,1		;make sure sticky bit dit not drop off

fround12:
	inc	SI		;adjust exponent

fround1:
	_ifs	 SI l 0ffh, fround11
	jmp	fpackOverflow

fround11:
	_ifs SI g 0, fround7
	dec	SI		;adjustment so it will shift ok
	call	fright_justify	;shift right untell SI is zero

fround7:
	test	AL,01000000b
	jne	fround6
	test	AL,00111111b
	je	fround5

fround6:
	tst	SI
	jne	fround10
	or	__fe_cur_env.status,FE_UNDERFLOW
fround10:
	or	__fe_cur_env.status,FE_INEXACT	;no longer exact
	_ifs	 __fe_cur_env.round e  FE_TONEAREST, fround3
	_ifs	 __fe_cur_env.round e  FE_TOWARDZERO,fround5
	_ifs	 __fe_cur_env.round ne FE_DOWNWARD,fround2


	;FE_DOWNWARD
	test	EDI,sgn
	jns	fround5
	jmps	fround4


fround2:;FE_UPWORD
	test	EDI,sgn
	js	fround5
	jmps	fround4


fround3:;FE_TONEAREST
	test	AL,01000000b	;If guard bit is not set then
	jz	fround5		;no rounding is necessary
	test	AL,10111111b	;Test if odd or sticky bits are set
	jz	fround5


fround4:;round up
	add	EAX,80h
	tst	SI		;is exponent zero
	je	fround9		;yes

	test	EAX,sgn		;is msb still where is should be
	jz	fround5
	inc	SI
	shr	EAX,1

fround5:
	shr	EAX,7
	jmp	fpack

fround9:			;when exponent is zero
	test	EAX,40000000h	;is msb still where is should be
	jz	fround5		;yes
	inc	SI		;exponent is now 1
	jmps	fround5
fround	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Raise underflow/overflow exception
; Input:
;	DI	bit 15 is sign bit of result
;	AX	1 = underflow
;		2 = overflow
; Returns:
;	EAX   adjusted result

fexception proc near
	dec	EAX
	jz	fexcepToNearest		   ;0 is result for underflow
	mov	EAX,shortexp

	;Adjust infinity based on rounding mode
	;NEAREST	infinity with sign
	;DOWN		+ overflows to + largest finite, - overflows to -inf
	;UP		- overflows to - largest finite, + overflows to +inf
	;TOZERO		to signed largest finite

	_ifs	 __fe_cur_env.round e FE_TONEAREST, fexcepToNearest
	_ifs	 __fe_cur_env.round e FE_TOWARDZERO, fexcepTowardZero
	_ifs	 __fe_cur_env.round e FE_UPWARD, fexcepUpward

	tst	EDI
	js	fexcepToNearest
	jmps	fexcepTowardZero

fexcepUpward:
	tst	EDI
	jns	fexcepToNearest

	;Generate largest finite
fexcepTowardZero:
	mov	EAX,07F7FFFFFh

fexcepToNearest:
	and	EDI,sgn
	or	EAX,EDI		  ;install sign bit
	ret
fexception endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; get specal case index of  float
;	index = case(a)
; input:
;	[EAX] = a
; Output:
;	SI = index
;
public fget_dtype
fget_dtype proc near
	clr	ESI				;set to default
	test	EAX,shortexp
	jz	fget_dtype1

	mov	EDI,EAX
	and	EDI,shortexp			;mask exponent
	_ifs	EDI ne short_infinity, fget_dtype2 ;c is not NaN or infinite
	mov	EDI,EAX
	test	EDI,fqnan_bit			 ;bit must be set to be
	jz	fget_dtype3				;a quit NaN
	mov	SI,dtype_qnan					;c is a quite NaN
	ret

fget_dtype3:
	and	EDI,shorthid-1				      ;Mantissa must be zero to be infinite
	jz	fget_dtype4				;otherwise
	mov	SI,dtype_snan					;c is a signaling NaN
	ret

fget_dtype4:						;c is infinite
	mov	SI,dtype_infinite
	ret

fget_dtype1:
	test	EAX,shorthid-1
	jnz	fget_dtype2
		mov	SI,dtype_zero				;c is zero

fget_dtype2:
	ret
fget_dtype endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert from float to double.
; Input:
;	EAX = float
; Output:
;	[EDX,EAX] = double

func	_FLTDBL@
	_ifs	 __8087 e 0, fltDbl1	  ;if no 8087
	sub	ESP,4			 ;2 extra words
	push	EAX
	fld	dword ptr [ESP]		;load float into 8087
fltret2:
	fstp	qword ptr [ESP]
	fwait				;wait for it to finish
	_pop	<EAX,EDX>		;pop result
	ret
fltDbl1:
	_push	<ESI,EDI>
	call	fget_dtype
	shl	ESI,2
	jmp	dword ptr cs:fltDblIndex[ESI]


fltDblIndex label	dword
	dd	fltDblNormal		;other
	dd	fltDblZero		;zero
	dd	fltDblInfinite		;infinite
	dd	fltDblSNaN		;SNaN
	dd	fltDblQNaN		;QNaN

fltDblNormal:
		call	funnorm			;unpack the float
	mov	EDX,EAX
	clr	EAX
	add	SI,longbias-shortbias	;fix the bias on the exponent
	call	dround			;pack a double
	jmps	fltDblDone

fltDblZero:
	clr	EDX
	jmps	fltDblSign

fltDblInfinite:
	mov	EDX,long_infinity
	jmps	fltDblSign
	
fltDblSNaN:
	mov	EDX,long_infinity OR 200h
	jmps	fltDblSign

fltDblQNaN:
	mov	EDX,long_qnan OR 040000h
	test	EAX,10000h
	je	fltDblSign
	or	EDX,0200h


fltDblSign:
	and	EAX,sgn
	or	EDX,EAX
	clr	EAX

fltDblDone:
	_pop	<EDI,ESI>
	ret
c_endp	_FLTDBL@

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert from double to float.
; Input:
;	[EDX,EAX]
; Output:
;	[EAX]
;	ECX,EBX   destroyed
;	ESI,EDI   preserved

func	_DBLFLT@

	_ifs	 __8087 e 0, dblFlt1	  ;if no 8087
	_push	<EDX,EAX>		;push double
	push	EBP
	mov	EBP,ESP
	fld	qword ptr 4[EBP]	;load b into 8087
	fstp	dword ptr 8[EBP]	;store float result
	pop	EBP
	add	ESP,4
	fwait				;wait for it to finish
	pop	EAX			;pop result
	ret

dblFlt1:
	_push	<ESI,EDI>
	call	dget_dtype
	shl	SI,2
	jmp	dword ptr cs:dblFltIndex[ESI]

dblFltIndex label	word
	dd	dblFltNormal		;other
	dd	dblFltSign		;zero
	dd	dblFltInfinite		;infinite
	dd	dblFltSNaN		;SNaN
	dd	dblFltQNaN		;QNaN

dblFltNormal:
	call	dunnorm			;unpack double
	sub	SI,longbias-shortbias	;fix exponent bias
	tst	EAX
	jz	dblFlt4
	or	EDX,1			;save sticky bit
dblFlt4:
	mov	EAX,EDX
	call	fround			;pack float
	jmps	dblFltDone



dblFltInfinite:
	mov	EAX,short_infinity
	jmps	dblFltSign
	
dblFltSNaN:
	mov	EAX,short_infinity OR 10000h
	jmps	dblFltSign

dblFltQNaN:
	mov	EAX,short_qnan
	cmp	DH,02h
	jne	dblFltSign
	or	EAX,10000h

dblFltSign:
	and	EDX,sgn
	or	EAX,EDX

dblFltDone:
	_pop	<EDI,ESI>
	ret
c_endp	_DBLFLT@




	endcode	double

	end
  
  
