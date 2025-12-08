;_ exec2.asm   Tue May 29 1990   Modified by: Walter Bright */
; DOS386 support added by G. Eric Engstrom
; SWAP support added by G. Eric Engstrom
; Copyright (C) 1985-1990 by Walter Bright
; All Rights Reserved

    ifndef _SWAP
include	macros.asm
    endif

	begdata

	extrn	_errno:word

param_block	equ $
env		dd	?			; offset address of environment
envseg  	dw      ?                       ; segment address of environment
comml   	dd      ?                       ; DWORD ptr to command line
commlseg	dw	?			; segment of command line

	enddata

	begcode	exec2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Execute a command.
; Use:
;	int _exec(command,commandline,envseg,chain)
;	char *command;		/* name of program to run		  */
;	char *commandline;	/* command line (128 bytes max preceeded  */
;				/* by a byte count and ended with a \r)	  */
;				/* (an extra 0 at the end is recommended) */
;	int envseg;		/* segment of environment		  */
;	int chain;		/* if !=0, then 'chain' to program, and	  */
;				/* terminate program when command terminates */
;
; Returns:
;	-1	error (_errno has the MS-DOS error code)
;	n	success (n is the exit status of the executed command)


	c_public	_exec
func	_exec
	push	EBP
	mov	EBP,ESP
	uses	<EBX,ECX,EDX,ESI,EDI,ES,DS>
	mov	EAX,P+SIZEPTR+SIZEPTR[EBP]	; get segment of environment
	mov	env,EAX
	mov	envseg,DS

	mov	ESI,P+SIZEPTR[EBP]		; SI = commandline
	mov	comml,ESI			; offset of command line
	mov	commlseg,DS

	mov	EDX,P[EBP]			; EDX -> command
	mov	AX,DS
	mov	ES,AX
	mov	EDX,P[EBP]
	mov	EBX,offset DGROUP:param_block		; EBX -> parameter block
	mov	EAX,4B00h		; load/execute program
	bdos				; perform exec
L6:	cld				; no direction flag bugs
	jc	L1			; if (error)

	bdos	4Dh			; get exit status of sub-process
	movzx	EAX,AX
L3:
	unuse	<DS,ES,EDI,ESI,EDX,ECX,EBX>
	pop	EBP
	ret

L1:	mov	_errno,AX		;save error code in _errno
	sbb	EAX,EAX			;return (-1)
	jmp	L3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Chain to command
L4:
	hlt				;not implemented

c_endp	_exec

	endcode	exec2

	end
