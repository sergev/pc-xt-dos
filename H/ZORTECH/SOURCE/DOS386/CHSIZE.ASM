;_ chsize.asm	22 May 1990   Modified by: Bob Stout */
; All Rights Reserved
; Written by Bob Stout
; Remainder copyright(C) 1990 by Walter Bright
; All Rights reserved

include macros.asm

;Permission modes
S_IWRITE	equ	80h
S_IREAD		equ	0100h

    if LCODE
	extrn	_write:far
	extrn	_lseek:far
    else
	extrn	_write:near
	extrn	_lseek:near
    endif


	begdata
	extrn	_errno:dword
	enddata


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
	push	EBP
	mov	EBP,ESP
	push	0			;SEEK_SET
	push	P+4[EBP]		;push offset
	push	P[EBP]			;push fd
	call	_lseek			;seek to position
	add	ESP,12
	_ifs	EAX e -1, done		;if error
	push	0			;push 0-byte write length
	push	EAX			;push dummy address
    if LPTR
	push	EAX
    endif
	push	P[EBP]			;push fd
	call	_write			;write zero bytes to truncate
	add	ESP,4+SIZEPTR+4
	_ifs	EAX e -1, done		;if error
	clr	EAX			;no, return 0 for success
done:
	mov	ESP,EBP
	pop	EBP
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
	push	EBP
	mov	EBP,ESP
      if LPTR
	push	DS
	lds	EDX,P[EBP]	;get filename
      else
	mov	EDX,P[EBP]
      endif
	mov	CL,P+SIZEPTR[EBP]
	shl	CL,1		;put S_IWRITE bit into Carry bit
	sbb	ECX,ECX
	inc	ECX		;turn into read-only bit in bit 0
	mov	EAX,4301h	;set file attribute
	bdos
      if LPTR
	pop	DS
      endif
	jnc	short C2	;no error
	movzx	EAX,AX		;clear high word of EAX
	mov	_errno,EAX	;error code
C2:	sbb	EAX,EAX
	pop	EBP
	ret

c_endp	chmod

	endcode	chsize

	end
