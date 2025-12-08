;_ strtoul.asm   Modified by: Walter Bright */
; Copyright (C) 1991 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

	include	macros.asm
	include stdio.asm

	begcode	strtoul

;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert ascii string to long.
; Use:
;	unsigned long strtoul(const char *p,char **pend,int base);

    if SPTR
pend	equ	word ptr P+SIZEPTR[BP]
    else
pend	equ	dword ptr P+SIZEPTR[BP]
    endif
base	equ	word ptr P+SIZEPTR+SIZEPTR[BP]

	c_public	strtoul

func	strtoul
	push	BP
	mov	BP,SP
	.save	<SI,DI>
    if SPTR
	mov	SI,P[BP]	;get p
    else
	push	DS
	lds	SI,P[BP]	;get p
    endif
A1:	lodsb
	.if	AL e ' ', A1
	.if	AL b 9, A5
	.if	AL be 0Dh, A1	;skip white space

A3:	.if	AL ne '+', A5
	lodsb

A5:	mov	BX,base
	tst	BX
	jz	BASE0
	.if	AL ne '0', A6
	lodsb
	.if	BX e 2, BASE2
	.if	BX e 16, BASE16
	jmp	A6


BASE2:	;Skip over '0b' and '0B'
	.if	AL e 'b', B16a
	.if	AL e 'B', B16a
	jmp	A6

BASE16:	;Skip over '0x' and '0X'
	.if	AL e 'x', B16a
	.if	AL ne 'X', A6
B16a:	lodsb
	jmp	A6

BASE0:	;Determine which base to use
	mov	base,10			;default to base 10
	.if	AL ne '0', A6		;it is decimal
	lodsb
	mov	base,8			;could be octal
	.if	AL e 'b', BIN
	.if	AL e 'B', BIN
	.if	AL e 'x', HEX
	.if	AL e 'X', HEX
	jmp	A6

HEX:	mov	base,16+6
BIN:	sub	base,6		;convert to base 2 or base 16
	lodsb

A6:	clr	DI		;DI accumulates error indication
	mov	CX,DI
	mov	DX,CX		;DX,CX = 0: accumulate result in DX,CX
	jmp	A8

ALPHA:	and	AL,0DFh		;to upper case
	sub	AL,'A'-10	;to 10..35
	jmp	A9

	even
A4:	lodsb
A8:	cbw			;AH = 0 if AL >= 0
	tst	AL
	jle	A2
	.if	AL ae 'A', ALPHA
	sub	AL,'0'		;to binary

A9:	.if	AX ae base, A2

	;DX,CX = DX,CX * base + AL
	mov	BX,AX
	mov	AX,DX
	mul	base			;AX = DX * base
	adc	DI,0
	xchg	AX,CX
	mul	base			;DX,AX = CX * base
	add	DX,CX
	adc	DI,0

	mov	CX,BX
	add	CX,AX
	adc	DX,0
	adc	DI,0

	jmp	A4

overflow:
    ifdef _MT
	mov	AX,ERANGE
    if LPTR
	pop	DS
	errno_set
	push	DS
    else
	errno_set
    endif
    else
    if LPTR
	pop	DS
	mov	_errno,ERANGE
	push	DS
    else
	mov	_errno,ERANGE
    endif
    endif
	mov	AX,-1
	cwd				;return ULONG_MAX
	jmp	A7

A2:	mov	AX,CX
	tst	DI			;any overflow?
	jne	overflow		;yes
A7:
    if SPTR
	mov	BX,pend
    else
	mov	CX,DS
	lds	BX,pend
    endif
	tst	BX
	jz	A11
	dec	SI			;back up to point to last char read
	mov	[BX],SI			;*pend = SI
    if LPTR
	mov	2[BX],CX		;*pend = CX:SI
    endif

A11:
    if LPTR
	pop	DS
    endif
	.restore <DI,SI>
	pop	BP
	ret
c_endp	strtoul

	endcode	strtoul

	end
