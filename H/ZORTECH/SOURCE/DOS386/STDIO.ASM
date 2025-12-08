;_ stdio.asm   Thu Mar  8 1990   Modified by: Walter Bright */
; Copyright (C) 1985-1990 by Walter Bright	*/
; All rights reserved.				*/

; Standard I/O header file	*/

EOF	equ	-1

BUFSIZ		equ	4096

_ptr	equ	0
_cnt	equ	_ptr + SIZEPTR
_base	equ	_cnt + 4
_flag	equ	_base + SIZEPTR
_file	equ	_flag + 4
_bufsiz	equ	_file + 4

_IOREAD		equ	1
_IOWRT		equ	2
_IONBF		equ	4
_IOMYBUF	equ	8
_IOEOF		equ	10h
_IOERR		equ	20h
_IOLBF		equ	40h
_IORW		equ	80h
_IOTRAN		equ	100h

;Values for errno
ENOENT		equ	2
EACCES		equ	5
ENOMEM		equ	8
EINVAL		equ	22
EDEADLOCK	equ	24h
EEXIST		equ	80
ERANGE		equ	1003

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Thread data structure (one is allocated per thread)
;Must match __thread1 in cinit.asm and struct THREAD in mt.h

thread_t struc
	t_stackoff	dd	0
;	t_stackseg	dw	0
	t_stacktop	dd	0
	t_errno		dd	0
    if SPTR
	t_strtok	dd	0
    else
	t_strtok	df	0
    endif
	t_tm		dd	9 dup (0)	;struct tm

	;Actually fenv_t
	t_fe_status	dw	0
	t_fe_control	dw	0
	t_fe_round	dw	0
	t_fe_res1	dw	0
	t_fe_res2	dw	0

	t_asctime	db	26 dup (0)
	t_digstr	db	32 dup (0)	;DBL_DIG*2+2
thread_t ends

THDSIZE	equ	(size thread_t)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ifdef _MT

  ifndef noerrno
    if LCODE
	extrn	__errno_set:far
    else
	extrn	__errno_set:near
    endif
  endif

errno_set macro	value
    ifnb <value>
	mov	EAX,value
    endif
	call	__errno_set
	endm

else

 ifndef noerrno
	begdata
	extrn	_errno:dword
	enddata
 endif

errno_set macro	value
    ifnb <value>
	mov	_errno,value
    else
	mov	_errno,EAX
    endif
	endm

endif

