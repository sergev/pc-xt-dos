;_ getthd.asm

include macros.asm

noerrno	equ	1
include stdio.asm
include flthead.asm

	begdata

	extrn	__thread1:word
	extrn	_errno:word

    ifdef _MT
	extrn	__linfoseg:word

    if SPTR
thdtbl		dw	0
		dw	offset DGROUP:__thread1
		dw	62 dup (0)
    else
thdtbl		dd	0
		dw	offset DGROUP:__thread1
		dw	DGROUP
		dd	62 dup (0)
    endif
thdtblmax	dw	64
    else
	extrn	__fe_cur_env:word
    endif

	enddata

    if LCODE
	extrn	_calloc:far
    else
	extrn	_calloc:near
    endif

	begcode	getthd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get pointer to thread data for current thread.
;	struct THREAD *_getthreaddata();
; Returns:
;	NULL	can't get thread data

	c_public _getthreaddata
func	_getthreaddata
    ifdef _MT
	mov	ES,__linfoseg		;get local information segment
	mov	BX,ES:06h		;get current thread ID
	.if	BX ae thdtblmax, L1	;if out of range

    if SPTR
	shl	BX,1
	mov	AX,thdtbl[BX]
	or	AX,AX
    else
	shl	BX,1
	shl	BX,1

	mov	AX,word ptr thdtbl[BX]
	mov	DX,word ptr thdtbl+2[BX]

	mov	CX,AX
	or	CX,DX
    endif
	jz	L2			;no thread data
	ret

L2:	;Create thread data
	push	BX
	pushi	AX,1
	pushi	AX,THDSIZE
	call	_calloc
	add	SP,4
	pop	BX

    if SPTR
	or	AX,AX
    else
	mov	CX,AX
	or	CX,DX
    endif
	jz	L1			;no memory

	mov	word ptr thdtbl[BX],AX
    if LPTR
	mov	word ptr thdtbl+2[BX],DX
    endif
	ret

L1:	clr	AX
    if LPTR
	cwd
    endif

    else ;single threaded
	mov	AX,offset DGROUP:__thread1
    if LPTR
	mov	DX,DS
    endif
    endif
	ret
c_endp	_getthreaddata

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set errno based on value in AX.
; Preserve contents of all registers and flags.

	public __errno_set
    if LCODE
__errno_set proc far
    else
__errno_set proc near
    endif

    ifdef _MT
	pushf
	.push	<BX,CX,DX,ES>
	push	AX
	callm	_getthreaddata
    if SPTR
	mov	BX,AX
	pop	AX
	mov	t_errno[BX],AX
    else
	mov	ES,DX
	mov	BX,AX
	pop	AX
	mov	ES:t_errno[BX],AX
    endif
	.pop	<ES,DX,CX,BX>
	popf
    else
	mov	_errno,AX	;for link compatibility with non-_MT code
    endif
	ret
__errno_set endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Retrieve pointer to where errno is stored.
;	int *_errno();

	public __errno

    if LCODE
__errno	proc	far
    else
__errno	proc	near
    endif

    ifdef _MT
      if LCODE
	push	CS
      endif
	call	near ptr __getthreaddata
	add	AX,t_errno
    else
      if LPTR
	mov	DX,DS
      endif
	mov	AX,offset DGROUP:_errno
    endif
	ret

__errno	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set exception bits.
; No registers modified except AX.

	public __FEEXCEPT
    if LCODE
__FEEXCEPT proc far
    else
__FEEXCEPT proc near
    endif
	WINENTER
    ifdef _MT
	.push	<BX,CX,DX,ES>
	push	P[BP]
	callm	_getthreaddata
      if SPTR
	mov	BX,AX
	pop	AX
	or	t_fe_status[BX],AX
      else
	mov	ES,DX
	mov	BX,AX
	pop	AX
	or	ES:t_fe_status[BX],AX
      endif
	.pop	<ES,DX,CX,BX>
    else
	mov	AX,P[BP]
	or	__fe_cur_env.status,AX
    endif
	WINLEAVE
	ret	2
__FEEXCEPT endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get rounding mode in AX.
; No registers modified.

	public __FEROUND
    if LCODE
__FEROUND proc far
    else
__FEROUND proc near
    endif
    ifdef _MT
	WINENTER_NF
	.push	<BX,CX,DX,ES>
	callm	_getthreaddata
      if SPTR
	mov	BX,AX
	mov	AX,t_fe_round[BX]
      else
	mov	ES,DX
	mov	BX,AX
	mov	AX,ES:t_fe_round[BX]
      endif
	.pop	<ES,DX,CX,BX>
	WINLEAVE_NF
    else
	mov	AX,__fe_cur_env.round
    endif
	ret
__FEROUND endp

	endcode	getthd

	end
