;_ controlc.asm  written by G. Eric Engstrom

;controlc_open			turn control C trapping on
;controlc_close			turn control C trapping off

include	macros.asm

	comm	__controlc_handler:dword	;address of handler

	begdata
        db 512 dup (?)
controlCStack equ $

controlCStackPtr	dd ?
controlCStackSeg	dw ?

cBreak1BVector		dd ?
cBreak1BVectorProtected	dd ?
			dw ?
cBreak23Vector		dd ?
cBreak23VectorProtected dd ?
			dw ?
cBreakStatus   db ?
	enddata

	begcode	controlc

	c_public cBreakHandler
func	cBreakHandler
        mov	controlCStackSeg,SS
        mov	controlCStackPtr,ESP
        push	DS
        pop	SS
        mov	ESP,offset DGROUP:controlCStack-SIZEPTR
        and	ESP,0FFFFFFFCh			;dword align stack
        sti

	push    EAX
	push    EBX
	push    ECX
	push    EDX
	push    EBP
	push    ESI
	push    EDI
	push    ES
	push    DS

	cmp	dword ptr __controlc_handler,0
	je	nohandler
	  call	  dword ptr __controlc_handler
nohandler:
 
	pop     DS
	pop     ES
	pop     EDI
	pop     ESI
	pop     EBP
	pop     EDX
	pop     ECX
	pop     EBX
	pop     EAX
        mov	SS,controlCStackSeg
        mov	ESP,controlCStackPtr
	iretd
c_endp	cBreakHandler

	c_public controlc_open
func	controlc_open
	push	EBP
	push	EBX
	push	DS
	push	ES
	; get current ^C BREAK STATUS
	mov	AX,03300h
	int	21h
	mov	cBreakStatus,DL
	; set current ^C BREAK STATUS TO ON
	mov	AX,03301h
	mov	DL,1
	int	21h

;	trap int 1b and 23
	push	ESI
	push	EDI

	mov	CL,01Bh
	mov	ESI,offset DGROUP:cBreak1BVector
	call	getVector
	mov	CL,023h
	mov	ESI,offset DGROUP:cBreak23Vector
	call	getVector

	mov	CL,01Bh
	mov	EDX,offset cBreakHandler
	call	installVector
	mov	CL,023h
	mov	EDX,offset cBreakHandler
	call	installVector

	pop	EDI
	pop	ESI

	pop	ES
	pop	DS
	pop	EBX
	pop	EBP
	ret
c_endp	controlc_open

	c_public controlc_close
func	controlc_close
	push	EBP
	push	EBX
	push	DS
        push	ES

        mov	CL,01Bh
        mov	ESI,offset DGROUP:cBreak1BVector
        call	restoreVector
        mov	CL,023h
	mov	ESI,offset DGROUP:cBreak23Vector
        call	restoreVector

	mov	AX,03301h
	mov	DL,cBreakStatus
	int	21h

	pop	ES
	pop	DS
	pop	EBX
	pop	EBP
	ret
c_endp	controlc_close

getVector proc near
	push	ESI
	push	ECX
	mov	AX,02502h	;get protected mode interrupt vector
	int	21h
	pop	ECX
	pop	ESI

	mov	4[ESI],EBX
	mov	8[ESI],ES

	push	ESI
	mov	AX,02503h	;get real mode interrupt vector
	int	21h
	pop	ESI

	mov	[ESI],EBX
	ret
getVector endp

restoreVector proc near
	push	DS
        mov	EBX,[ESI]
	lds	EDX,4[ESI]
	mov	AX,02507h	;set real and protected mode interrupt vectors
L1:	int	21h
        pop	DS
        ret
restoreVector endp

installVector proc near
	push	DS
        push	CS
        pop	DS
	mov	AX,02506h
	jmp	short L1
installVector endp

	endcode controlc

	end

