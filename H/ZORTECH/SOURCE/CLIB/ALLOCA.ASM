;_ alloca.asm	Tue Aug 21 1990   Modified by: Walter Bright */
;Copyright (C) 1990-1991 by Walter Bright
;All Rights Reserved
;Written by Walter Bright

include	macros.asm

    ifdef _WINDOWS
;Offsets into stack segment maintained by Windows
STACKTOP	equ	0ah
STACKMIN	equ	0ch
STACKBOT	equ	0eh
    endif

    ifndef _WINDOWS
    if SPTR
	begdata
	extrn	__BASE:word,__heapbottom:word
	enddata
    endif
    endif

	begcode	alloca

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate data on the stack frame.
; This is a 'magic' function that needs help from the compiler to
; work right, do not change its name, do not call it from other compilers!
; Input:
;	P-2[SP]	number of bytes to allocate (nbytes)
;	-2[BP]	number of bytes in the locals in the stack frame
;		(this parameter is passed magically by the ZTC code
;		 generator, thus do not use alloca() with ZTCs prior
;		 to 2.15).
;		This is adjusted upon return to reflect the additional
;		size of the stack frame.
; Returns:
;	DX:AX	allocated data, NULL if nbytes is 0 or stack overflows

	c_public	alloca
func	alloca
	push	SI
	mov	BX,SP
	mov	BX,SS:P[BX]	;get nbytes
	inc	BX
	and	BL,0FEh		;round up to word

	neg	BX
	jz	Aoverflow	;alloca(0) returns NULL
	mov	SI,BX		;SI = -nbytes
	add	BX,SP
	jae	Aoverflow

    ifdef _WINDOWS
	.if	SS:STACKTOP a BX, Aoverflow
    else
    if SPTR
	.if	BX be __BASE, Aoverflow		;is SP off bottom?
	.if	BX ae __heapbottom, Aoverflow	;has SP wrapped around?
    endif
    endif
	xchg	BX,SP

	;We must copy down to [SP] the temps on the stack.
	;The number of temps is (BP - BX - locals).

	mov	CX,BP
	sub	CX,BX
	sub	CX,-2[BP]		;CX = number of temps
	mov	AX,CX
;	jz	L1			;no temps
	shr	CX,1			;to count of words in temps
					;(always at least 3)
	even
L2:	mov	DX,SS:[BX]
	mov	SS:[BX+SI],DX
	inc	BX
	inc	BX
	loop	L2

L1:	sub	-2[BP],SI		;adjust locals by nbytes for next
					; call to alloca()

	;The offset of the allocated area is (SP + temps)
	add	AX,SP
	mov	DX,SS

	pop	SI
	ret

Aoverflow:
	;We overflowed the stack. Return NULL
	clr	AX
	cwd
	pop	SI
	ret
c_endp	alloca

	endcode	alloca

	end
