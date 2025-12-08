;_ d16.asm
; Make D16 calls work in real mode, so we don't need to ifdef
; all the calls.

include	macros.asm

ifdef I8086L

	begcode	D16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;

	public	_D16SegAbsolute

_D16SegAbsolute	proc	far
	push	BP
	mov	BP,SP

	mov	DX,P[BP]
	mov	AX,P+2[BP]
	mov	CX,4
L1:	shr	AX,1
	rcr	DX,1
	loop	L1

	pop	BP
	ret
_D16SegAbsolute	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;

	public	_D16SegCancel

_D16SegCancel	proc	far
	ret
_D16SegCancel	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;

	public	_D16SegCSAlias

_D16SegCSAlias	proc	far
	push	BP
	mov	BP,SP

	mov	AX,P[BP]
	mov	DX,P+2[BP]

	pop	BP
	ret
_D16SegCSAlias	endp

	endcode	D16

endif

	end

