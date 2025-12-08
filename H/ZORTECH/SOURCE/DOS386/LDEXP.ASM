;_ LDEXP.ASM  May 8 1991   Modified by: Phil Hinger */
; Written by Walter Bright
; Copyright (C) 1984-1991 by Walter Bright
; All rights reserved

	include	macros.asm
	include flthead.asm

	.287

	begdata
	extrn	__8087:word

	extrn	__fe_cur_env:word

	enddata


	begcode double

	extrn	dunnorm:near, dround:near, dget_dtype:near, exception:near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	double ldexp(value,exp)
;	double value;
;	int exp;
; Returns:
;	value*(2**exp)

	c_public ldexp


func	ldexp
	push	EBP
	mov	EBP,ESP
	_ifs	__8087 e 0, Ld2		;if 8087 not installed
	fild	word ptr P+8[EBP]	;load exp
	fld	qword ptr P[EBP]		;load value
	fscale				;ST(0) = ST(0) * (2**ST(1))
	fstp	qword ptr P[EBP]
	fstp	ST			;leave stack as we found it
					;(also doing an fwait, MASM doesn't
					; recognize fnstp !@#$%^&)
	mov	EDX,P+4[EBP]
	mov	EAX,P+0[EBP]		;transfer result to EDX,EAX
	pop	EBP
	ret

Ld2:	uses	<ESI,EDI>
	mov	EAX,P+0[EBP]
	mov	EDX,P+4[EBP]		;transfer double to EDX,EAX

	call	dget_dtype
	shl	ESI,2
	jmp	dword ptr cs:LdIndex[ESI]

LdIndex label	    word
	dd	LdNormal	    ;other
	dd	LdZero		    ;zero
	dd	LdInfinite	    ;infinite
	dd	LdSNaN		    ;SNaN
	dd	LdQNaN		    ;QNaN

LdNormal:
	mov	ESI,EDX
	and	ESI,longexp		;mask off exponent bits
	je		LdSubNormal
 
	xor	EDX,ESI			;clear exponent bits in EDX
	shr	ESI,20			;right justify exponent
	add	ESI,P+8[EBP]		;add exp
	jle	Ld7			;test for underflow

	cmp	ESI,7ffh		;test for  overflow
	jge	Ld6			;yes

	shl	ESI,20
;	and	ESI,longexp		;dump extraneous bits (not necessary)
	or	EDX,ESI			;install exponent
;	jmps	LdDone

LdZero:
LdInfinite:
LdQNaN:

LdDone:
	unuse	<EDI,ESI>
	pop	EBP
	ret

Ld6:	;overflow
	mov	EDI,EDX
	or	__fe_cur_env.status,FE_OVERFLOW or FE_INEXACT
	call	exception	;raise overflow exception
	jmps	LdDone

Ld7:	;underflow

	; AX,BX,CX,DX <<= 11	 normalize so dround will work
	mov	EDI,EDX		 ;save sign bit
	or	EDX,longhid
	shld	EDX,EAX,11
	shl	EAX,11
	call	dround
	jmps	LdDone


LdSNaN:
	or	EAX,dqnan_bit
	or	__fe_cur_env.status,FE_INVALID
	jmps	LdDone


LdSubNormal: ; do it the slow but safe way
	call	dunnorm			;unpack
	add	ESI,P+8[EBP]		 ;add exp
	call	dround
	jmps	LdDone

c_endp	ldexp

	endcode double

	end
