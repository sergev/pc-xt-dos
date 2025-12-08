;_ stdio.asm   Sat Feb  6 1988   Modified by: Walter Bright */
; Copyright (C) 1985-1988 by Walter Bright	*/
; All rights reserved.				*/

; Standard I/O header file	*/

EOF	equ	-1

BUFSIZ		equ	512
    IF SPTR
BIGBUF		equ	40 * BUFSIZ
    ENDIF

_ptr	equ	0
_cnt	equ	_ptr + SIZEPTR
_base	equ	_cnt + 2
_flag	equ	_base + SIZEPTR
_file	equ	_flag + 2
_bufsiz	equ	_file + 2
    IFDEF	BIGBUF
_seg	equ	_bufsiz + 2
    ENDIF

_IOREAD		equ	1
_IOWRT		equ	2
_IONBF		equ	4
_IOMYBUF	equ	8
_IOEOF		equ	10h
_IOERR		equ	20h
_IOLBF		equ	40h
_IORW		equ	80h
_IOTRAN		equ	100h
    IFDEF	BIGBUF
_IOBIGBUF	equ	400h
    ENDIF


;Values for errno
ENOENT		equ	2
EACCES		equ	5
ENOMEM		equ	8
EINVAL		equ	22
EDEADLOCK	equ	36
EEXIST		equ	80
ERANGE		equ	1003

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Thread data structure (one is allocated per thread)
;Must match __thread1 in cinit.asm and struct THREAD in mt.h

thread_t struc
	t_stackoff	dw	0
	t_stackseg	dw	0
	t_stacktop	dw	0
	t_errno		dw	0
    if SPTR
	t_strtok	dw	0
    else
	t_strtok	dd	0
    endif
	t_tm		dw	9 dup (0)	;struct tm

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
	mov	AX,value
    endif
	call	__errno_set
	endm

else

 ifndef noerrno
	begdata
	extrn	_errno:word
	enddata
 endif

errno_set macro	value
    ifnb <value>
	mov	_errno,value
    else
	mov	_errno,AX
    endif
	endm

endif
