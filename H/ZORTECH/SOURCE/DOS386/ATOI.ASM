;_ atoi.asm   Modified by Joe Huffman June 25, 1990
; Copyright (C) 1985-1991 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

	include	macros.asm

	begcode	atoi

;;;;;;;;;;;;;;;;;;;;;;;;;
; Take absolute value of integer.

	public  _abs
;	c_public abs   ;Phar Lap's 386asm doesn't like this for some reason.

func	abs
	;Fall through to labs
c_endp	abs

;;;;;;;;;;;;;;;;;;;;;;;;;
; Take absolute value of long.

	c_public labs
func	labs
	_align
	push	EBP
	mov	EBP,ESP
	uses	<edx>
	mov	EAX,P[EBP]
	cdq
	xor	EAX,EDX
	sub	EAX,EDX
	unuse	<edx>
	pop	EBP
	ret
c_endp	labs

;;;;;;;;;;;;;;;;;;;;;;;;;
; Convert ascii string to long/integer.
; Use:
;	i = atoi(p);
;	i = atol(p);

	;Same as atoi().
	c_public atol
func	atol
c_endp	atol

	c_public atoi
func	atoi
	_align
	push	EBP
	mov	EBP,ESP
	uses	<EBX,ECX,ESI>
   if SPTR
	mov	ESI,P[EBP]	;get p
    else
	push	DS
	lds	ESI,P[EBP]	;DS:ESI -> string
    endif
	cld
A1:	lodsb
	_if	AL e ' ', A1
	_if	AL b 9, A3
	_if	AL be 0Dh, A1	;skip white space

A3:	clr	ECX		;accumulated result
	mov	EBX,ECX		;assume positive
	movzx	EAX,AL
	_if	AL e '+', A4
	_if	AL ne '-', A5
	dec	EBX		;neg flag (BX = -1)
	align	4
A4:	lodsb
A5:	sub	AL,'0'		;to binary
	jb	A2		;not a digit
	_if	AL ae 10, A2	;not a digit

	;CX = CX * 10 + AX
	lea	ECX,[ECX+ECX*4]
	add	ECX,ECX
	add	ECX,EAX

	jmp	A4

A2:	mov	EAX,ECX
	xor	EAX,EBX
	sub	EAX,EBX		;if (BX == -1), negate AX
    if LPTR
	pop	DS
    endif
	unuse   <ESI,ECX,EBX>
	pop	EBP
	ret
c_endp	atoi

	endcode	atoi

	end
