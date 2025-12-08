;_ jmp.asm   Mon Nov 5 1990   Modified by: Joe Huffman */
; Copyright (C) 1985-1990 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

	include	macros.asm

	begcode	jmp

	public	_setjmp,_longjmp

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Locations in jmp_buf[]:

_IP	equ	0
_CS	equ	4
_BP	equ	8
_SP	equ	12
_SS	equ	16
_BX	equ	20
_SI	equ	24
_DI	equ	28
_ES	equ	32
_DS	equ	36

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Use:
;	setjmp(jmp_buf env);

_setjmp	proc	near
	mov	ECX,PS[ESP]
	mov	_SP[ECX],ESP	;save old registers
	mov	_BP[ECX],EBP
	mov	_BX[ECX],EBX
	mov	_SI[ECX],ESI
	mov	_DI[ECX],EDI
	mov	_CS[ECX],CS
	mov	_SS[ECX],SS
	mov	_ES[ECX],ES
	mov	_DS[ECX],DS
	mov	EAX,[ESP]
	mov	_IP[ECX],EAX	;save return address offset
	clr	EAX		;always returns 0
	ret
_setjmp	endp

;;;;;;;;;;;;;;;;;;;;;;;;;
; Use:
;	longjmp(jmp_buf env,val)

_longjmp	proc	near
	pop	EAX		;throw away return address
	pop	ECX		;get env
	pop	EAX		;AX = val
	tst	EAX
	jnz	short L1
	inc	EAX		;don't allow val == 0
L1:
	mov	SS,_SS[ECX]
	mov	ESP,_SP[ECX]	;restore environment
	add	ESP,PS		;compensate for ret addr
	mov	EBP,_BP[ECX]
	mov	EBX,_BX[ECX]
	mov	ESI,_SI[ECX]
	mov	EDI,_DI[ECX]
	mov	ES,_ES[ECX]
	push	_CS[ECX]
	push	_IP[ECX]
	mov	DS,_DS[ECX]
	db	0CBh		;386ASM doesn't support RETF
_longjmp	endp

	endcode	jmp
	end

