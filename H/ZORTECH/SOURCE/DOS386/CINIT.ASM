;_ cinit.asm   June 20, 1990  Modified by: Joe Huffman
; Copyright (C) 1985-1990 by Walter Bright
; All rights reserved.
; Written by Joe Huffman
; C initialization for Phar Lap DOS386 and SCO UNIX System V version 3.2.0

;define DOS386 for Phar Lap, default is SCOUNIX

include macros.asm
include flthead.asm

	extrn _write:near

	public	__doserrno,__osmode,_errno,__dodtors
        public  __argc,__argv,__cinit,__envptr
	public	__8087

	begdata

ifdef DOS386
	public __psp,__osmajor,__dos,__osminor,__DOS386_version,__DOS386_environ

__psp		 dw	SEG_PSP ;segment of program segment prefix
__osmajor label	 byte		;MSC compatibility
__dos		 db	?	;MS-DOS major version number
__osminor label	 byte		;MSC compatibility
		 db	?	;MS_DOS minor version number
__DOS386_version dd	?
__DOS386_environ dd	?
else
	public __environ
__environ	dd	?	;Pointer to the environment (char *_environ[]).
endif

__8087		dd	0
__envptr	dd	?	;Pointer to the environment (char *_envptr).

;Globals for argc and argv, so they are accessible and modifiable by
;static constructors.
__argc		dd	?	;number of args
__argv		dd	?	;filled in by C.ASM
__osmode	db	1	;= 1 if in protected mode

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;The layout of this must match struct THREAD,
;as it is the thread 1 data

		public	__thread1
__thread1	label	word
		dd	0
;		dw	0
		dd	0
;__oserr	label	word		;Lattice C compatibility
__doserrno	label	word		;DOS error number (for compatibility
					; with MSC). It is the same as errno.
_errno		dd	0		;global error number

    ifdef _MT
    if SPTR
		dd	0		;t_strtok
    else
		df	0		;t_strtok
    endif
		dd	9 dup (0)	;t_tm
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

	begcode cinit

__cinit proc	near
	call	doctors		;perform static constructors
	ret
__cinit endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Perform static constructors
; Go backwards through the list, so we do what was linked in last, first.

doctors proc	near
	uses	<ECX,EDI>

	;Call ctors
	mov	EDI,offset DGROUP:XIE
CT1:	_ifs	EDI be <offset DGROUP:XIB>,CT2
	  sub	  EDI,SIZEPTR
	  mov	  ECX,[EDI]
	  jecxz	  short CT1	  ;skip null pointers
	  call	  ECX
	  jmps	  CT1

CT2:
	unuse	<EDI,ECX>
	ret
doctors endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Perform static destructors
; Go in the reverse order that the constructors were called.

__dodtors	proc	near
	uses	<ecx,edi>
	;Call near dtors
	mov	edi,offset DGROUP:XCB
DT1:	_if	edi ae <offset DGROUP:XCE>,DT2
	mov	ecx,[edi]
	jecxz	short DT1	;skip null pointers
	call	ecx
	add	edi,4
	jmps	DT1

DT2:	unuse	<edi,ecx>
	ret

__dodtors	endp

	endcode cinit

	end

