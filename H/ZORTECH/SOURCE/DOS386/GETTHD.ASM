;_ getthd.asm

include macros.asm

noerrno	equ	1
include stdio.asm
include flthead.asm

	begdata

	extrn	__thread1:word
	extrn	_errno:dword

    ifdef _MT
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
    else ;single threaded
	mov	EAX,offset DGROUP:__thread1
    if LPTR
	clr	EDX
	mov	DX,DS
    endif
    endif
	ret
c_endp	_getthreaddata

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set errno based on value in EAX.
; Preserve contents of all registers and flags.

	public __errno_set
    if LCODE
__errno_set proc far
    else
__errno_set proc near
    endif

    ifdef _MT
	pushf
	.push	<EBX,ECX,EDX,ES>
	push	EAX
	callm	_getthreaddata
    if SPTR
	mov	EBX,EAX
	pop	EAX
	pop	t_errno[EBX],EAX
    else
	mov	ES,DX
	mov	EBX,EAX
	pop	EAX
	mov	ES:t_errno[EBX],EAX
    endif
	.pop	<ES,EDX,ECX,EBX>
	popf
    else
	mov	_errno,EAX	;for link compatibility with non-_MT code
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
	add	EAX,t_errno
    else
      if LPTR
	clr	EDX
	mov	DX,DS
      endif
	mov	EAX,offset DGROUP:_errno
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
	push	EBP
	mov	EBP,ESP
    ifdef _MT
	.push	<EBX,ECX,EDX,ES>
	push	P[EBP]
	callm	_getthreaddata
      if SPTR
	mov	EBX,EAX
	pop	EAX
	or	t_fe_status[EBX],AX
      else
	mov	ES,DX
	mov	EBX,EAX
	pop	EAX
	or	ES:t_fe_status[EBX],AX
      endif
	.pop	<ES,EDX,ECX,EBX>
    else
	mov	EAX,P[EBP]
	or	__fe_cur_env.status,AX
    endif
	pop	EBP
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
	.push	<EBX,ECX,EDX,ES>
	callm	_getthreaddata
      if SPTR
	mov	EBX,EAX
	movzx	EAX,t_fe_round[EBX]
      else
	mov	ES,DX
	mov	EBX,EAX
	movzx	EAX,ES:t_fe_round[EBX]
      endif
	.pop	<ES,EDX,ECX,EBX>
    else
	movzx	EAX,__fe_cur_env.round
    endif
	ret
__FEROUND endp

	endcode	getthd

	end
