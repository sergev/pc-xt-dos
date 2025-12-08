;_ find.asm   Modified by: Walter Bright */
; Copyright (C) 1985-1991 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

include	macros.asm

	begdata

	c_extrn	_doserrno,word

FIND	label	byte		;struct used by findfirst() and findnext()
reserved	db	21 dup(0)	;reserved by DOS
attribute	db	0		;attribute found (FA_XXXX)
time		dw	0
date		dw	0		;file's time and date
sizel		dw	0		;file's size
sizeh		dw	0		;file's size
fname		db	13 dup(0)	;filename followed by 0 byte
		db	?
	enddata

	begcode	find

;;;;;;;;;;;;;;;;;;;;;;
; Find first matching file.
; Input:
;	name	Drive, path and filename of file to be found. May
;		include global filename characters.
;	attribute	Attribute of file to search for. Attributes
;		are described in the MS-DOS manual. The search strategy
;		is described under DOS call 0x11.
; Returns:
;	Pointer to a static struct FIND.
;	If error, NULL is returned and _doserrno is set to the error #.
;

	c_public findfirst
func	findfirst
	push	EBP
	mov	EBP,ESP
	mov	EDX,offset DGROUP:FIND
	bdos	1Ah			;set DTA
	mov	EDX,P[EBP]		;DS:DX = name
	mov	ECX,P+SIZEPTR[EBP]	;CX = attribute
	bdos	4Eh			;find first
	jmps	FN2
c_endp	findfirst

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Find next matching file.
; Returns:
;	pointer to static struct FIND
;	NULL if no more matching files found

	c_public findnext
func	findnext
	push	EBP
	mov	EBP,ESP

	mov	EDX,offset DGROUP:FIND
	bdos	1Ah			;set DTA
	bdos	4Fh			;findnext
FN2:	jc	FN1			;if error
FNX:
	mov	EAX,offset DGROUP:FIND	;return &FIND
	pop	EBP
	ret

FN1:	;error, return NULL
	mov	_doserrno,AX
FN3:	clr	EAX
	pop	EBP
	ret
c_endp	findnext

	endcode	find

	end

