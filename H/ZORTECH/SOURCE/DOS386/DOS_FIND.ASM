;_ dos_find.asm   22 May 1990   Modified by: Bob Stout */
; Copyright (C) 1985-1990 by Walter Bright
; All Rights Reserved
; Written by Walter Bright, Bob Stout

include	macros.asm

	begdata

	extrn	__doserrno:word

FIND		STRUC	;struct used by _dos_findfirst() and _dos_findnext()
reserved	db	21 dup(0)	;reserved by DOS
attribute	db	0		;attribute found (FA_XXXX)
time		dw	0
date		dw	0		;file's time and date
sizel		dw	0		;file's size
sizeh		dw	0		;file's size
fname		db	13 dup(0)	;filename followed by 0 byte
FIND		ENDS
	enddata

	begcode	dosfind

;;;;;;;;;;;;;;;;;;;;;;
; Find first matching file.
; Input:
;	name	Drive, path and filename of file to be found. May
;		include global filename characters.
;	attribute	Attribute of file to search for. Attributes
;		are described in the MS-DOS manual. The search strategy
;		is described under DOS call 0x11.
;       ffblk   Pointer to a FIND structure.
; Returns:
;	Zero if successful, else _doserrno.
;

	c_public _dos_findfirst
func	_dos_findfirst
	push	EBP
	mov	EBP,ESP
    if LPTR
	push	DS			;DS:EDX = &ffblk
	lds	EDX,P+SIZEPTR+4[EBP]
	bdos	1Ah			;set DTA
	lds	EDX,P[EBP]		;DS:EDX = name
    else
	mov	EDX,P+SIZEPTR+4[EBP]
	bdos	1Ah			;set DTA
	mov	EDX,P[EBP]		;DS:EDX = name
    endif
	mov	ECX,P+SIZEPTR[EBP]	;CX = attribute
	bdos	4Eh			;find first
    if LPTR
	pop	DS
    endif
	jmps	FN2
c_endp	_dos_findfirst

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Find next matching file.
; Input:
;       ffblk   Pointer to a FIND structure.
; Returns:
;	Zero if successful, else _doserrno.

	c_public _dos_findnext
func	_dos_findnext
	push	EBP
	mov	EBP,ESP

    if LPTR
    	push	DS
	lds	EDX,P[EBP]
	bdos	1Ah			;set DTA
    	pop	DS
    else
	mov	EDX,P[EBP]
	bdos	1Ah			;set DTA
    endif
	bdos	4Fh			;findnext
FN2:	jc	FN1			;if error
	clr	EAX			;return zero
	pop	EBP
	ret

FN1:	;error, return _doserrno
	mov	DGROUP:__doserrno,AX
	pop	EBP
	ret
c_endp	_dos_findnext

	endcode	dosfind

	end
