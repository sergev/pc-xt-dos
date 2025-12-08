;_ cinit.asm   Mon Dec 11 1989   Modified by: Walter Bright */
; Windows support added by G. Eric Engstrom Jan 1991	-D_WINDOWS
; Copyright (C) 1985-1991 by Walter Bright
; All rights reserved.
; Written by Walter Bright
; C initialization

include	macros.asm
include flthead.asm

	public	__psp
	public	__tab_size
	public	_aenvseg
    ifndef DOS16RM
	public	__datapar
	public	__heapbottom
	public	__pastdata
    endif
	public	__doserrno,_errno
;	public	__oserr
	public	__dos,__dodtors
	public	__osmajor,__osminor,__osmode
	public	__argc,__argv
	c_public  _exit,_cinit

;Function pointers to far constructors
XIFB	segment	word public 'DATA'
XIFB	ends
XIF	segment	word public 'DATA'
XIF	ends
XIFE	segment	word public 'DATA'
XIFE	ends

    ife LCODE
;Function pointers to near constructors
XIB	segment	word public 'DATA'
XIB	ends
XI	segment	word public 'DATA'
XI	ends
XIE	segment	word public 'DATA'
XIE	ends

;Function pointers to near destructors
XCB	segment	word public 'DATA'
XCB	ends
XC	segment	word public 'DATA'
XC	ends
XCE	segment	word public 'DATA'
XCE	ends
    endif

;Function pointers to far destructors
XCFB	segment	word public 'DATA'
XCFB	ends
XCF	segment	word public 'DATA'
XCF	ends
XCFE	segment	word public 'DATA'
XCFE	ends

;Stuff all these segments into one group so they can all be accessed by DS
    ife LCODE
DGROUP	group	XIFB,XIF,XIFE,XIB,XI,XIE,XCB,XC,XCE,XCFB,XCF,XCFE,_DATA,CONST,_BSS
    else
DGROUP	group	XIFB,XIF,XIFE,XCFB,XCF,XCFE,_DATA,CONST,_BSS
    endif

    ifdef __OS2__
	includelib OS2.LIB
	extrn	DOSGETVERSION:far
	extrn	DOSGETMACHINEMODE:far
	extrn	DOSEXIT:far
	extrn	DOSWRITE:far
    endif

	begdata

__psp	dw	?			;segment of program segment prefix
__osmajor label	byte			;MSC compatibility
__dos		db	?		;MS-DOS major version number
__osminor label	byte			;MSC compatibility
		db	?		;MS_DOS minor version number
__osmode	db	0		;= 1 if in protected mode
    ifdef _MT
    ifdef _WINDOWS
semmsg		db	'Semaphore error',0
    else
semmsg		db	0Dh,0Ah,'Semaphore error',0Dh,0Ah,0
    endif
    endif

	even

__tab_size	dw	8	; default size of tabs in this system, see tabsize.h
_aenvseg	dw	0	; Selector of Environment segment
				; (DLL's don't have an environment segment)
    ifndef DOS16RM
__datapar	dw	?	;# of paragraphs currently in data segment
				; (max is 0FFFh)
__pastdata	dw	?	;offset of 1 past data segment
__heapbottom	dw	?	;lowest offset in heap (used to detect
				; free() errors)
    endif

;Globals for argc and argv, so they are accessible and modifiable by
;static constructors.
__argc	dw	?			;number of args
__argv	dw	?			;filled in by C.ASM
    if LPTR
	dw	seg DGROUP
    endif

    ifndef __OS2__
;Stuff for overlay handler
	comm	__intno:byte	;interrupt number that calls overlay service routine
	comm	__ovlflag:byte	;set to !=0 if overlay handler is installed
	comm	__ovlvec:dword	;previous overlay interrupt vector
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;The layout of this must match struct THREAD,
;as it is the thread 1 data

		public	__thread1
__thread1	label	word
		dw	0
		dw	0
		dw	0
;__oserr	label	word		;Lattice C compatibility
__doserrno	label	word		;DOS error number (for compatibility
					; with MSC). It is the same as errno.
_errno		dw	0		;global error number

    ifdef _MT
    if SPTR
		dw	0		;t_strtok
    else
		dd	0		;t_strtok
    endif
		dw	9 dup (0)	;t_tm
    endif

	public	__fe_cur_env
__fe_cur_env	fenv_t	<>		;current floating point environment

    ifdef _MT
		db	26 dup (0)	;t_asctime
		db	32 dup (0)	;t_digstr
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	public	_FE_DFL_ENV
_FE_DFL_ENV	fenv_t	<>		;default floating point environment

	enddata

	begcode	cinit

; Far entry point for DLL initialization code
	public	__cinit_f
__cinit_f proc	far
    ife LCODE
    	WINENTER_NF
	call	_cinit
        WINLEAVE_NF
	ret
    endif
__cinit_f endp

func	_cinit
    	WINENTER_NF
	push	DI

	;Figure out what OS version we're running under.
	;We already did this in C.ASM, but do it again for MS Windows' benefit
    ifdef __OS2__
	push	DS
	pushi	AX,<offset DGROUP:__dos>
	call	DOSGETVERSION
	mov	AX,word ptr __dos
	xchg	AH,AL
	mov	word ptr __dos,AX	;swap halves

	;Find out if we are in real mode
	
	push	DS
	pushi	AX,<offset DGROUP:__osmode>
	call	DOSGETMACHINEMODE
    else
	bdos	30h
	mov	word ptr __dos,AX
    endif

	call	doctors		;perform static constructors
	pop	DI
        WINLEAVE_NF
	ret
c_endp	_cinit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Terminate execution of C program.
; Input:
;	2[SP]	error code (ignored if not >= MS-DOS 2.00)

func	_exit
    ifdef __OS2__
	mov	BP,SP
	pushi	AX,1		;1 = terminate all threads in this process
	push	P-2[BP]		;error code
	call	DOSEXIT
    else
    	WINENTER_NF
	;Terminate overlay handler if it's installed
	.if	__ovlflag e 0, E2	;if not installed
	mov	AL,__intno		;get overlay interrupt number
	push	DS
	lds	DX,__ovlvec		;DS:DX = previous vector
	mov	AH,25h
	bdos
	pop	DS
E2:
    ifndef _WINDOWS
	bdos	30h		;get DOS version number
	.if	AL b 2, E1	;if pre-DOS 2.00
    endif
	mov	BP,SP
	mov	AL,P-2[BP]	;AL = error code
	bdos	04Ch		;Terminate a process (Exit)

E1:
    ifndef _WINDOWS
	push	__psp
	xor	AX,AX
	push	AX		;push &(PSP:0)
	.retf			;far return to DOS
    endif
    endif
c_endp	_exit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Perform static constructors
; Go backwards through the list, so we do what was linked in last, first.

doctors	proc	near
	;Call far ctors
	mov	DI,offset DGROUP:XIFE
CT2:	.if	DI be <offset DGROUP:XIFB>,CT1
	sub	DI,4
	mov	AX,[DI]
	or	AX,2[DI]
	jz	CT2				;skip null pointers
	call	dword ptr [DI]
	jmp	CT2

CT1:
    ife LCODE			;no near ctors if M or L models
	;Call near ctors
	mov	DI,offset DGROUP:XIE
CT3:	.if	DI be <offset DGROUP:XIB>,CT4
	dec	DI
	dec	DI
	mov	CX,[DI]
	jcxz	CT3		;skip null pointers
	call	CX
	jmp	CT3
    endif

CT4:	ret
doctors	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Perform static destructors
; Go in the reverse order that the constructors were called.

func	_dodtors
	WINENTER_NF
    ife LCODE			;no near dtors if M or L models
	;Call near dtors
	mov	DI,offset DGROUP:XCB
DT3:	.if	DI ae <offset DGROUP:XCE>,DT1
	mov	CX,[DI]
	jcxz	DT3		;skip null pointers
	call	CX
	inc	DI
	inc	DI
	jmp	DT3
    endif

DT1:	;Call far dtors
	mov	DI,offset DGROUP:XCFB
DT2:	.if	DI ae <offset DGROUP:XCFE>,DT4
	mov	AX,[DI]
	or	AX,2[DI]
	jz	DT2				;skip null pointers
	call	dword ptr [DI]
	add	DI,4
	jmp	DT2

DT4:	WINLEAVE_NF
	ret
c_endp	_dodtors endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Print out fatal message and terminate program.
; Input:
;	DS:DX	points to ASCIZ message

	public	___faterr
___faterr proc	near
    ifdef __OS2__
	;Determine length of message in CX
	push	DS
	pop	ES
	mov	DI,DX		;ES:DI -> string
	clr	AX
	mov	CX,-1
	repne	scasb
	not	CX
	dec	CX

	mov	AX,2
	sub	SP,AX		;leave space for return value
	mov	BX,SP		;and remember address
	push	AX		;stderr file handle
	push	DS		;message segment
	push	DX		;message offset
	push	CX		;message length
	push	SS		;return value segment
	push	BX		;return value offset
	call	DOSWRITE
    else
    ifdef _WINDOWS
	push	DS
	push	DX
	call	MESSAGEFATALERROR
    else
	;Get pointer to end of string
	push	DS
	pop	ES
	mov	DI,DX		;ES:DI -> string
	clr	AX
	mov	CX,-1
	repne	scasb

	dec	DI
	mov	byte ptr [DI],'$'	;terminate string with $
	bdos	9
    endif
    endif
	mov	AX,1		;error exit code
	push	AX
	call	__exit		;abort
	;never reached
___faterr endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Something went wrong with the semaphores.
; Print an error message and abort the program.

    ifdef _MT
	c_public _semerr
func	_semerr
	mov	AX,offset DGROUP:semmsg
	jmp	near ptr ___faterr
c_endp	_semerr
    endif

	endcode	cinit

	end
