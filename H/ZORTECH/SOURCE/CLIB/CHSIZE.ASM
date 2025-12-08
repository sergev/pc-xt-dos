;_ chsize.asm	22 May 1990   Modified by: Bob Stout */
; DOS chsize() is Copyright (C) 1990 by Robert B. Stout dba MicroFirm
; All Rights Reserved
; Written by Bob Stout
; Windows support added by G. Eric Engstrom Dec 1990
; Remainder copyright(C) 1990 by Walter Bright
; All Rights reserved

include macros.asm
include stdio.asm

;Permission modes
S_IWRITE	equ	80h
S_IREAD		equ	0100h

    ifdef __OS2__
	extrn DOSNEWSIZE:far
	extrn DOSSETFILEMODE:far
    else
    if LCODE
	c_extrn write,far
	c_extrn lseek,far
    else
	c_extrn write,near
	c_extrn lseek,near
    endif
    endif


	begcode	chsize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  chsize
;
;  Truncate or extend a file opened under DOS
;
;  Arguments: 1 - file handle
;	      2 - Position for truncation
;
;  Returns: 0 if successful, else -1
;
;  int _cdecl chsize(int fd, long posn);

	c_public chsize
func	chsize
    ifdef __OS2__
	push	BP
	mov	BP,SP
	push	P[BP]		;file handle
	push	P+4[BP]
	push	P+2[BP]		;new file size
	call	DOSNEWSIZE	;set new size
C3:	tst	AX		;success?
	jz	C1		;yes
	errno_set
	mov	AX,-1
C1:	pop	BP
    else
	WINENTER_VCM
	push	SI
	push	DI
	mov	SI,p[BP]		;get fd in SI
	mov	DI,0ffffh		;save const -1 in DI
	clr	AX
	push	AX			;SEEK_SET
	push	P+4[BP]			;push offset
	push	P+2[BP]
	push	SI			;push fd
	callm	lseek			;seek to position
	add	SP,8
	cmp	DX,DI			;error?
	jne	cont			;no, go on
	cmp	AX,DI			;maybe...
	je	done			;yes, abort
cont:
	clr	AX
	push	AX			;push 0-byte write length
	push	AX			;push dummy address
    if LPTR
	push	AX
    endif
	push	SI			;push fd
	callm	write			;write zero bytes to truncate
	add	SP,sizeptr+4
	cmp	AX,DI			;error?
	je	done			;yes, all done
	inc	DI			;no, return 0 for success
done:
	mov	AX,DI
	pop	DI
	pop	SI
	mov	SP,BP
	WINLEAVE_VCM
    endif
	ret

c_endp	chsize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	#include <sys\stat.h>
;	#include <io.h>
;	int chmod(char *filename, int mode);
; Change file access mode to that specified by mode.
; Mode is one of the following values:
;	S_IREAD			Set file to read-only.
;	S_IWRITE		Set file to write-only.
;	S_IREAD | S_IWRITE	Set file to read or write.
; S_IREAD is ignored under DOS.
; Return Value
;	0 means chmod was successful. -1 means it is not,
;	and errno is set.
; See Also
;	access, fstat, creat, stat, open

	c_public chmod
func	chmod
    ifdef __OS2__
	push	BP
	mov	BP,SP
    if LPTR
	push	P+2[BP]
    else ; LPTR
	push	DS
    endif ;LPTR
	push	P[BP]		;pathname of file
	mov	AL,P+SIZEPTR[BP]
	shl	AL,1		;put S_IWRITE bit into Carry bit
	sbb	AX,AX
	inc	AX		;turn into read-only bit in bit 0
	push	AX
	clr	AX
	push	AX
	push	AX		;dword of 0s
	call	DOSSETFILEMODE
	jmp	C3
    else ;__OS2__
	WINENTER
    if LPTR
	push	DS
	lds	DX,P[BP]	;get filename
    else ;LPTR
	mov	DX,P[BP]
    endif ;LPTR
	mov	CL,P+SIZEPTR[BP]
	shl	CL,1		;put S_IWRITE bit into Carry bit
	sbb	CX,CX
	inc	CX		;turn into read-only bit in bit 0
	mov	AX,4301h	;set file attribute
	bdos
    if LPTR
	pop	DS
    endif ;LPTR
	jnc	C2		;no error
	errno_set
C2:	sbb	AX,AX
	WINLEAVE
	ret
    endif ;__OS2__
c_endp	chmod

	endcode	chsize

	end
