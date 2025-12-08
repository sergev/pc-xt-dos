;_ vcma.asm   Mon Jan 29 1990   Modified by: Walter Bright
; Windows support added by G. Eric Engstrom Jan 1991	-D_WINDOWS
; Copyright (C) 1989-1990 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

; Assembly glue routines for overlay manager

include macros.asm

ifndef _WINDOWS
ifdef I8086V

	extrn	_vcm_callgate : far
	extrn	_vcm_gate : far

	begdata

axsave	dw	?
bxsave	dw	?
cxsave	dw	?
dxsave	dw	?

	extrn	_gates:word
	extrn	_gate_freelist:word
	enddata

	begcode	VCM

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Called with the following sequence:
;	int	3Fh
;	db	vcmnum
;	dw	offset

	public	_vcm_isr
_vcm_isr proc	far
	sti			;the interrupt turned off interrupts

	;Save the registers
	mov	axsave,AX
	mov	bxsave,BX
	mov	cxsave,CX
	mov	dxsave,DX

	pop	BX
	pop	ES		;ES:BX -> overlay data record
	add	SP,2		;faster than a POPF

	;Get parameters
	mov	AL,ES:[BX]	;get overlay number
	clr	AH
	mov	CX,ES:1[BX]	;save offset into overlay

	;Decide if we should skip putting the return address on the stack,
	;i.e. if this is the equivalent of a 'jmp' rather than a 'call'.
	mov	DX,ES
	.if	DX e <_gates[2]>, isgate
	.if	DX ne <seg _vcm_gate>, noskip
	.if	BX ae <offset _vcm_gate>, skip

	;Replace return address
noskip:	push	ES
	add	BX,3		;point past end of overlay data record
	push	BX

skip:
	;We need a stack frame (just in case the caller is an overlay, and
	;gets moved by vcm_callgate)
	inc	BP
	push	BP
	mov	BP,SP

	push	CX		;save offset into overlay segment

	push	AX
	call	_vcm_callgate	;(onum)
	add	SP,2

	pop	CX

	;Undo the stack frame
	pop	BP
	dec	BP

	push	AX		;segment of overlay
	push	CX		;offset into overlay

	;Restore registers
	mov	DX,dxsave
	mov	CX,cxsave
	mov	BX,bxsave
	mov	AX,axsave

	ret			;jump to overlay

	even
isgate:
	; The caller was a gate that was on the free list.
	; Return the gate to the free list.
	dec	BX
	dec	BX			;point back to beginning of gate
	mov	byte ptr ES:[BX],1	;g->f.type = GATE_FREE
	mov	DX,_gate_freelist
	mov	ES:2[BX],DX
	mov	DX,_gate_freelist + 2
	mov	ES:4[BX],DX		;g->f.next = gate_freelist
	mov	_gate_freelist,BX
	mov	_gate_freelist+2,ES	;gate_freelist = g
	jmp	skip

_vcm_isr endp

	endcode	VCM

endif ;I8086V
endif ;_WINDOWS

	end
