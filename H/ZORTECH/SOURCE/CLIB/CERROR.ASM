;_ cerror.asm
; Windows support added by G. Eric Engstrom Jan 1991	-D_WINDOWS
; OS2 support added by G. Eric Engstrom May 1989	-D__OS2__
; Copyright (C) 1990 by Concentric Software Designs, Inc.
; All Rights Reserved
; Written by G. Eric Engstrom

;cerror_open			turn critical error trapping on
;cerror_close			turn critical error trapping off

include	macros.asm

    ifndef __OS2__
    ifndef _WINDOWS

	begdata

	db 1024 dup (?)
cerrorStack equ $

cerrorStackSeg	dw ?
cerrorStackPtr	dw ?
cerrorInt24Vector	dd ?
cerrorAX	dw ?
cerrorDI	dw ?

	public	__cerror_handler

__cerror_handler	dd	0
	enddata
    endif ;_WINDOWS
    endif ;__OS2__

	begcode	cerror
    ifndef _WINDOWS
    ifndef __OS2__

    ifndef DOS16RM
cerrorDS	dw ?
    endif

	assume DS:DGROUP
cerrorIntTrap proc far
	sti
	push	DS
	push	AX
    ifdef DOS16RM
	mov	AX,seg DGROUP
	mov	DS,AX
    else
	mov	AX,CS:cerrorDS
        mov	DS,AX
    endif
	pop	AX
	mov	DGROUP:cerrorStackSeg,SS
	mov	DGROUP:cerrorStackPtr,SP
	mov	DGROUP:cerrorAX,AX
	mov	DGROUP:cerrorDI,DI
	cli
	mov	AX,DS
	mov	SS,AX
	mov	SP,offset DGROUP:cerrorStack-2
	sti

	push	BP
	push	BX
	push	CX
	push	DX
	push	SI
	push	DI
	push	ES

	mov	BX,offset DGROUP:cerrorDI
	push	DS
	push	BX
	mov	BX,offset DGROUP:cerrorAX
	push	DS
	push	BX
	cmp	word ptr __cerror_handler+2,0
	je	nohandler
	  call	  dword ptr __cerror_handler
nohandler:
	add	sp,8
	mov	AX,DGROUP:cerrorAX

	pop	ES
	pop	DI
	pop	SI
	pop	DX
	pop	CX
	pop	BX
	pop	BP

	cli
	mov	SS,DGROUP:cerrorStackSeg
	mov	SP,DGROUP:cerrorStackPtr
	sti
	pop	DS
	iret
cerrorIntTrap endp

	c_public cerror_open
func	cerror_open
	push	BP
	push	DS
	push	ES
    ifndef DOS16RM
	mov	CS:cerrorDS,DS
    endif
	mov	AX,03524h
	int	21h
	mov	word ptr DGROUP:cerrorInt24Vector,BX
	mov	word ptr DGROUP:cerrorInt24Vector+2,ES
	push	CS
	pop	DS
	mov	DX,offset CS:cerrorIntTrap
	mov	AH,025h
	int	21h
	pop	ES
	pop	DS
	pop	BP
	ret
c_endp	cerror_open

	c_public cerror_close
func	cerror_close
	push	BP
	push	DS
	lds	DX,dword ptr DGROUP:cerrorInt24Vector
	mov	AX,02524h
	int	21h
	pop	DS
	pop	BP
	ret
c_endp	cerror_close

    endif ; __OS2__
    endif ; _WINDOWS

	endcode cerror

	end

