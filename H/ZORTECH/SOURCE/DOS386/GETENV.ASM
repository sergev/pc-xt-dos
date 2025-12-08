;_ getenv.asm   Sun Dec 10 1989   Modified by: Walter Bright */
; DOS386 supported added by G. Eric Engstrom
; Copyright (C) 1985-1990 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

	include	MACROS.ASM

	begdata
	extrn	__envptr:dword
	enddata

	c_extrn	malloc,near

	begcode	getenv

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get environment string
; Return pointer to it.

	c_public getenv
func	getenv
	push	EBP
	mov	EBP,ESP
	uses	<ES,EBX,ESI,EDI>
   	mov	EDI,dword ptr __envptr

	;Now look for the environment string
	;EDI -> environment
    ife ESeqDS
	mov	AX,DS
        mov	ES,AX
    endif
	clr	EAX			;0 used for the .ifs, scasb and error
					; return value!
	cld
	mov	ECX,-1			;arbitrarilly large count
	mov	EBX,ECX			;BX = -1 (save cycles on indexing)
L10:	  mov	  ESI,P[EBP]		;SI -> name we're looking for
	  repe	  cmpsb			;scan till first mismatch
	  _ifs	  <[EBX + EDI]> e AL, L3	;end of environment string, no match
	  _ifs	  <[EBX + ESI]> e AL, L7	;possible match
L8:	  repne	  scasb			;scan for terminating 0
	  jmp	  L10
L7:	_ifs	<byte ptr [EBX + EDI]> ne '=', L8
	  mov     EAX,EDI                 ;point to value of environment var
L3:	unuse	<EDI,ESI,EBX,ES>
	pop	EBP
	ret
c_endp	getenv

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Initialize _envptr

if 0
	c_public _getenv_init
func	_getenv_init
	mov	EAX,offset DGROUP:dummy
	push	EAX
	callm	getenv
	add	ESP,SIZEPTR
	ret
c_endp	_getenv_init

static_ctor	_getenv_init
endif
	endcode	getenv

	end

