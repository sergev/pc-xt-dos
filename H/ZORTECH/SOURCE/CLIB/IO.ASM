;_ io.asm   Tue Dec 19 1989   Modified by: Walter Bright
; Windows support added by G. Eric Engstrom Dec 1990	-D_WINDOWS
; Modified by Bob Stout May 1990
; OS2 support added by Nikki Locke May 1989		-D__OS2__
; Copyright (C) 1985-1991 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

	include	macros.asm
	include stdio.asm

;Open flags
O_RDONLY	equ	0
O_WRONLY	equ	1
O_RDWR		equ	2
O_APPEND	equ	8
O_NOINHERIT	equ	80h
O_CREAT		equ	100h
O_TRUNC		equ	200h
O_EXCL		equ	400h

;Permission modes
S_IWRITE	equ	80h
S_IREAD		equ	0100h

    ifdef __OS2__
extrn	DOSREAD:far
extrn	DOSWRITE:far
extrn	DOSOPEN:far
extrn	DOSCLOSE:far
extrn	DOSMOVE:far
extrn	DOSDELETE:far
extrn	DOSCHGFILEPTR:far
extrn	DOSFINDFIRST:far
extrn	DOSFINDCLOSE:far
extrn	DOSQHANDTYPE:far
extrn	DOSQFILEMODE:far
    endif

	begdata
;	c_extrn	errno,word
	c_extrn _osmajor,byte
   ifdef _WINDOWS
	public	__pwincio_rw
    if LCODE
__pwincio_rw	dd	0
    else
__pwincio_rw	dw	0
    endif
    if SPTR
	public	__pwincio_rwx
    if LCODE
__pwincio_rwx	dd	0
    else
__pwincio_rwx	dw	0
    endif
    endif
   endif

	enddata

	begcode	io

	c_public read,write,open,sopen,close,creat
	c_public lseek,getDS,getES,getSS,filesize,rename,unlink

    ifndef __OS2__
	c_public dos_creat,dos_open
    endif
	c_public isatty,remove
    if SPTR
	c_public _readx,_writex
    endif

    ifdef __OS2__
;;;;;;;;;;;;;;;;;;;;;;;;;
; set up arguments fd,buffer,length for read(), write(), open() etc.
;
setargs	proc	near
	pop	DX			;return address
	push	BP
	mov	BP,SP
	sub	SP,AX			;reserve space for arguments
	push	DX			;put return address back
	mov	AX,P[BP]		;1st argument
	mov	BX,P+2[BP]
	mov	CX,P+4[BP]
	mov	DX,P+6[BP]		;4th argument
	ret
setargs	endp

;;;;;;;;;;;;;;;;;;;;;;;;;
; Read data from a file.
; Use:
;	read(fd,buffer,length)
; Returns:
;	-1	error
;	0	end of file
;	n	number of bytes actually read
;

func	read
	mov	AX,2			;reserve 2 bytes
	call	setargs
	push	AX			;fd = file handle
    if SPTR
	push	DS			;buffer segment
	push	BX			;buffer address
	push	CX			;length
    else
	push	CX			;buffer segment
	push	BX			;buffer offset
	push	DX			;length
    endif
F3:	lea	AX,[BP-2]
	push	SS
	push	AX			;save no of bytes written here
	call	DOSREAD
F2:	tst	AX
	jnz	F5			;error
	mov	AX,[BP-2]		;no of bytes written/read
F1:	mov	SP,BP
	pop	BP
	ret

F5:	errno_set			;save error number
	mov	AX,-1
	jmps	F1
c_endp	read


;;;;;;;;;;;;;;;;;;;;;;;;;
; Read data from a file.
; Use:
;	_readx(fd,buffer,length,segment)
; Returns:
;	-1	error
;	0	end of file
;	n	number of bytes actually read
;

    if SPTR
func	_readx
	mov	AX,2
	call	setargs
	push	AX			;fd = file handle
	push	DX			;buffer segment
	push	BX			;buffer offset
	push	CX			;length
	jmps	F3
c_endp	_readx
    endif


;;;;;;;;;;;;;;;;;;;;;;;;;
; Write data to a file.
; Use:
;	int write(fd,buffer,length)
; Returns:
;	-1	error
;	n	number of bytes actually written
;

func	write
	mov	AX,2
	call	setargs
	push	AX			;fd = file handle
    if SPTR
	push	DS			;buffer segment
	push	BX			;buffer address
	push	CX			;length
    else
	push	CX			;buffer segment
	push	BX			;buffer offset
	push	DX			;length
    endif
F4:	lea	AX,[BP-2]
	push	SS
	push	AX			;save no of bytes written here
	call	DOSWRITE
	jmps	F2
c_endp	write

;;;;;;;;;;;;;;;;;;;;;;;;;
; Write data to a file.
; Use:
;	int _writex(fd,buffer,length,segment)
; Returns:
;	-1	error
;	n	number of bytes actually written
;

    if SPTR
func	_writex
	mov	AX,2
	call	setargs
	push	AX			;fd = file handle
	push	DX			;buffer segment
	push	BX			;buffer offset
	push	CX			;length
	jmps	F4
c_endp	_writex
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;
; Return data segment.

func	getDS
	mov	AX,DS
	ret
c_endp	getDS

;;;;;;;;;;;;;;;;;;;;;;;;;
; Return extra segment.

func	getES
	mov	AX,ES
	ret
c_endp	getES

;;;;;;;;;;;;;;;;;;;;;;;;
; Return stack segment

func	getSS
	mov	AX,SS
	ret
c_endp	getSS

;;;;;;;;;;;;;;;;;;;;;;;;;
; Open a file
; Use:
;	int open(name,rwmode,pmode)
; Returns:
;	-1	error
;	fd	file handle
;

func	open
	mov	AX,4
	call	setargs
O8:
    if SPTR
	and	BL,8Fh			;sharing mode: deny none
	or	BL,40h
    else
	and	CL,8Fh			;sharing mode: deny none
	or	CL,40h
    endif
O1:
if SPTR
	push	BX			;save rwmode
	push	DS			;name segment
	push	AX			;name offset
else
	push	CX			;save rwmode
	push	BX			;name segment
	push	AX			;name offset
	mov	BX,CX			;rwmode in BX
	mov	CX,DX			;pmode in CX
endif
	;BX = rwmode
	;CX = pmode (file attribute)

	test	BX,O_CREAT
	jz	O2			;no

	mov	DX,10001b		;open if exist, create if not exist
	test	BX,O_EXCL
	jz	O5
	test	BX,O_TRUNC		;both O_EXCL and O_TRUNC
	jnz	Oerror			;is an invalid rwmode
	dec	DX			;fail if exist, create if not exist
	jmp	O6

Oerror:	mov	AX,EINVAL
	jmp	F2

O5:	test	BX,O_TRUNC
	jz	O6
	inc	DX			;replace if exist, create if not exist

O6:
; following lines are commented out so that PMODE argument is ignored (0)
;	test	CX,092h			;any WRITE permissions set ?
	mov	CX,0			;normal attribute
;	jnz	O4
;	inc	CX			;read only attribute
	jmp short O4

O2:
	clr	CX			;attribute = 0
	mov	DX,1			;open if exists, fail if not
	test	BX,O_TRUNC
	jz	O4
	inc	DX			;replace if exists, fail if not

O4:	;Dump unused bits in open mode
	and	BX,0f3h			;mask unused bits

	;ptr to file name is already on stack
	;BX = open mode
	;CX = attribute
	;DX = open flag

	lea	AX,[BP-2]
	push	SS
	push	AX			;address of file handle

	dec	AX
	dec	AX
	push	SS
	push	AX			;address of action taken

	xor	AX,AX
	push	AX
	push	AX			;primary allocation = 0
	push	CX			;attribute
	push	DX			;open flag
	push	BX			;open mode
	push	AX
	push	AX			;Reserved 0l
	call	DOSOPEN

	pop	BX			;restore rwmode
	tst	AX			;error?
	jnz	OX			;yes, nothing more to do
	mov	AX,[BP-2]		;error?
	and	BX,O_APPEND		;No, append?
	jz	O7			;No, all done
	push	AX			;Save handle
	pushi	BX,2			;Use SEEK_END
	clr	BX			;Offset = 0L
	push	BX
	push	BX
	push	AX			;fd
	callm	lseek
	add	SP,8
	pop	AX			;Restore handle...
	jmp	F1			; and all done

OX:	errno_set			;save DOS error number
	mov	AX,-1			;error
O7:	jmp	F1
c_endp	open

func	sopen
	push	BP
	mov	BP,SP
	sub	SP,4			;reserve space for arguments
	mov	AX,P[BP]		;1st argument
	mov	BX,P+2[BP]
    if SPTR
	and	BL,8Fh			;mask rwmode
	mov	CX,P+4[BP]		;get shmode...
	and	CL,70h			; and mask it
	or	BL,CL			;merge rwmode & shmode
	mov	CX,P+6[BP]		;pmode
    else
	mov	CX,P+4[BP]		;get rwmode...
	and	CL,8Fh			; and mask it
	mov	DX,P+6[BP]		;get shmode...
	and	DL,70h			; and mask it
	or	CL,DL			;merge rwmode & shmode
	mov	DX,P+8[BP]		;pmode
    endif
	jmp	O1
c_endp	sopen
;;;;;;;;;;;;;;;;;;;;;;;;;
; Create a file
; Use:
;	int creat(name,pmode)
; Returns:
;	-1	error
;	fd	file handle
;

func	creat
	mov	AX,4
	call	setargs
    if	SPTR
	mov	CX,BX				;attribute
	mov	BX,O_CREAT or O_TRUNC or O_RDWR	;replace if exists, create if not
    else
	mov	DX,CX				;attribute
	mov	CX,O_CREAT or O_TRUNC or O_RDWR	;replace if exists, create if not
    endif
	jmp	O8
c_endp	creat

;;;;;;;;;;;;;;;;;;;;;;;;;
; Close a file
; Use:
;	int close(fd)
; Returns:
;	-1	error
;	0	successful
;

func	close
	push	BP
	mov	BP,SP
	push	P[BP]			;file handle
	call	DOSCLOSE
C1:	test	AX,AX
	jz	C2
	errno_set
	mov	AX,-1
C2:
	pop	BP
	ret
c_endp	close

;;;;;;;;;;;;;;;;;;;;;;;;
; Rename a file.		J.K.H. 2/15/86
; Use:
;	int rename (from, to)
; Returns:
;	-1	error
;	0	successful
;

func	rename
	push	BP
	mov	BP,SP
if SPTR
	push	DS			;old path segment
	push	P[BP]			;old path offset
	push	DS			;new path segment
	push	P+2[BP]			;new path offset
else
	push	P+2[BP]			;old path segment
	push	P[BP]			;old path offset
	push	P+6[BP]			;new path segment
	push	P+4[BP]			;new path offset
endif
	xor	AX,AX
	push	AX
	push	AX			;Reserved 0l
	call	DOSMOVE
	jmp	C1
c_endp	rename

;;;;;;;;;;;;;;;;;;;;;;;;;
; Delete a file
; Use:
;	int unlink(name)
; Returns:
;	-1	error
;	0	successful
;

func	remove
c_endp	remove

func	unlink
	push	BP
	mov	BP,SP
if SPTR
	push	DS			;name segment
else
	push	P+2[BP]			;name segment
endif
	push	P[BP]			;name offset
	xor	AX,AX
	push	AX
	push	AX			;Reserved 0l
	call	DOSDELETE
	jmp	C1
c_endp	unlink

;;;;;;;;;;;;;;;;;;;;;;;;;
; Seek to specified file position.
; Use:
;	long lseek(int fd,long offset,int mode)
; Input:
;	mode =	SEEK_SET or SEEK_CUR or SEEK_END
; Returns:
;	-1L	error
;	n	new file position
;

func	lseek
	mov	AX,4
	call	setargs
	push	AX			;file handle
	push	CX
	push	BX			;new position
	push	DX			;mode
	lea	AX,[BP-4]
	push	SS
	push	AX			;where to put new position
	call	DOSCHGFILEPTR
	tst	AX			;error?
	jnz	L2			;yes
	mov	AX,[BP-4]
	mov	DX,[BP-2]		;new position
L1:	mov	SP,BP
	pop	BP
	ret

L2:	errno_set			;save DOS error number
	mov	AX,-1
	cwd
	jmps	L1
c_endp	lseek

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get and return the size of a file.
; Use:
;	long filesize(filename)
;	char *filename;
; Returns:
;	-1L	error

func	filesize
	push	BP
	mov	BP,SP
	sub	SP,40		;36 bytes for FileFindBuf
	mov	word ptr [BP-4],1	;no of entries to find
	mov	[BP-2],0FFFFh	;dir handle
if SPTR
	push	DS		;name segment
else
	push	P+2[BP]		;name segment
endif
	push	P[BP]		;name offset
	lea	AX,[BP-2]
	push	SS
	push	AX		;address of dir handle
	pushi	BX,6		;find all normal files, plus system and hidden
	lea	BX,[BP-40]	;FileFindBuf
	push	SS
	push	BX
	pushi	BX,36		;buffer length
	push	SS
	dec	AX
	dec	AX
	push	AX		;address of search count
	xor	AX,AX
	push	AX
	push	AX		;reserved 0l
	call	DOSFINDFIRST
	push	AX		;save error code
	push	word ptr [BP-2]	;directory handle
	call	DOSFINDCLOSE
	pop	BX		;get back previous error code
	mov	AX,[BP-40+12]
	mov	DX,[BP-40+14]	;file size
	test	BX,BX
	jz	S1		;no error
	mov	AX,BX
	errno_set
	mov	AX,-1
	cwd			;error return = -1L
S1:	mov	SP,BP
	pop	BP
	ret

c_endp	filesize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Determine if handle is a tty.
; Use:
;	int isatty(int fd)
; Returns:
;	!=0	character device
;	0	not a character device or error

func	isatty

	push	BP
	mov	BP,SP
	sub	SP,4
	push	P[BP]		;file handle
	lea	AX,[BP-4]
	push	SS
	push	AX		;address of handle type
	inc	AX
	inc	AX
	push	SS
	push	AX		;address of FlagWord
	call	DOSQHANDTYPE
	test	AX,AX
	jz	F9		;error
	errno_set
	xor	AX,AX
	jmps	F8

F9:	mov	BX,[BP-4]
	cmp	BX,1
	jnz	F8		;not a tty
	test	AX,00000011b
	jnz	F8		;not screen or keyboard
	inc	AX		;now AX = 1
F8:	mov	SP,BP
	pop	BP
	ret
c_endp	isatty


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	#include <io.h>
;	int access(char *filename,int mode);
; Synopsis:
;	Test file for existence and it's access bits. The access bits are:
;	#define F_OK	0	/* does file exist?	*/
;	#define X_OK	1	/* execute permission?	*/
;	#define W_OK	2	/* write permission?	*/
;	#define R_OK	4	/* read permission?	*/
;	OR these values together to test a combination.
; Bugs:
;	Since MS-DOS only records a write permission bit, the X_OK and
;	R_OK access permissions always succeed. They are included here
;	for unix compatibility.
; Returns:
;	0	file exists and access mode is granted
;	-1	otherwise and errno is set

	c_public access
func	access
	push	BP
	mov	BP,SP
	sub	SP,2			;make room for attribute
	mov	AX,SP

    if LPTR
	push	P+2[BP]			;filename segment
    else
	push	DS
    endif
	push	P[BP]			;filename offset

	push	SS			;push &attribute
	push	AX

	clr	AX			;push 0L
	push	AX
	push	AX

	call	DOSQFILEMODE
	tst	AX
	stc				;assume error
	jnz	A1			;error
	test	byte ptr P+SIZEPTR[BP],2 ;is W_OK set?
	jz	A2			;no
	shr	byte ptr -2[BP],1	;is file read-only?
	jnc	A2			;no
	mov	AX,EACCES
A1:	errno_set
A2:	sbb	AX,AX
	mov	SP,BP
	pop	BP
	ret
c_endp	access

    else ;__OS2__

;;;;;;;;;;;;;;;;;;;;;;;;;
; Write data to a file.
; Use:
;	int write(fd,buffer,length)
; Returns:
;	-1	error
;	n	number of bytes actually written
;

func	write
	mov	AH,40h			;write to file or device
	jmps	F4
c_endp	write

;;;;;;;;;;;;;;;;;;;;;;;;;
; Read data from a file.
; Use:
;	read(fd,buffer,length)
; Returns:
;	-1	error
;	0	end of file
;	n	number of bytes actually read
;

func	read
	mov	AH,3Fh			;read from file or device
F4:	WINENTER
	mov	BX,P[BP]		;BX = fd (file handle)
    ifdef _WINDOWS
	mov	CX, word ptr __pwincio_rw
    if LCODE
	or	CX, word ptr __pwincio_rw+2
    endif
	jcxz	RW1			;WINC is not loaded
	jmp	__pwincio_rw
RW1:
    endif
    if SPTR
	mov	DX,P+2[BP]		;DX = buffer address
	mov	CX,P+2+SIZEPTR[BP]	;CX = number of bytes to read/write
	bdos				;read/write from/to file or device
    else
	push	DS
	lds	DX,P+2[BP]		;DS:DX = buffer address
	mov	CX,P+2+SIZEPTR[BP]	;CX = number of bytes to read
	bdos				;read from file or device
	pop	DS
    endif
F2:	jnc	F1			;no error
	errno_set			;save DOS error number
	sbb	AX,AX			;error
F1:	WINLEAVE
	ret
c_endp	read


;;;;;;;;;;;;;;;;;;;;;;;;;
; Read data from a file.
; Use:
;	_readx(fd,buffer,length,segment)
; Returns:
;	-1	error
;	0	end of file
;	n	number of bytes actually read
;

    if SPTR
func	_readx
	mov	AH,3Fh			;read from file or device
F3:	WINENTER
	mov	BX,P[BP]		;BX = fd (file handle)
    ifdef _WINDOWS
	mov	CX, word ptr __pwincio_rwx
    if LCODE
	or	CX, word ptr __pwincio_rwx+2
    endif
	jcxz	RW2			;WINC is not loaded
	jmp	__pwincio_rwx
RW2:
    endif ;_WINDOWS
	mov	DX,P+2[BP]		;DX = buffer address
	mov	CX,P+4[BP]		;CX = number of bytes to read/write
	push	DS
	mov	DS,P+6[BP]		;DS = segment of buffer
	bdos				;read/write from file or device
	pop	DS
	jmps	F2
c_endp	_readx
    endif


;;;;;;;;;;;;;;;;;;;;;;;;;
; Write data to a file.
; Use:
;	int _writex(fd,buffer,length,segment)
; Returns:
;	-1	error
;	n	number of bytes actually written
;

    if SPTR
func	_writex
	mov	AH,40h			;write to file or device
	jmps	F3
c_endp	_writex
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;
; Return data segment.

func	getDS
	mov	AX,DS
	ret
c_endp	getDS

;;;;;;;;;;;;;;;;;;;;;;;;;
; Return extra segment.

func	getES
	mov	AX,ES
	ret
c_endp	getES

;;;;;;;;;;;;;;;;;;;;;;;;
; Return stack segment

func	getSS
	mov	AX,SS
	ret
c_endp	getSS

;;;;;;;;;;;;;;;;;;;;;;;;;
; Open a file (DOS mode)
; Use:
;	int dos_open(name,rwmode)
; Returns:
;	-1	error
;	fd	file handle
;

func	dos_open
	WINENTER
	mov	AH,3Dh			;open file
	mov	AL,P+SIZEPTR[BP]	;AL = rwmode (0,1,2)
OPEN1:
    if SPTR
	mov	DX,P[BP]		;DX -> name
	bdos
    else
	push	DS
	lds	DX,P[BP]		;DS:DX -> name
	bdos
	pop	DS
    endif
	jmp	F2
c_endp	dos_open

;;;;;;;;;;;;;;;;;;;;;;;;;
; Open a file
; Use:
;	int open(name,rwmode,pmode)
; Returns:
;	-1	error
;	fd	file handle
;

_bdos	proc	near
    if LPTR
	push	DS
	lds	DX,P[BP]		;filename
	bdos
	pop	DS
    else
	mov	DX,P[BP]		;filename
	bdos
    endif
	ret
_bdos	endp

func	open
	WINENTER
	sub	SP,4
	mov	AX,P+SIZEPTR[BP]	;AX = rwmode (0,1,2)
	and	AL,8Fh			;Mask off sharing bits...
;	or	AL,40h			; add sharing (deny none)...
	.if	_osmajor ge 3, DO4	;if DOS 3+?
	and	AL,7			;only these bits have meaning
DO4:	mov	-2[BP],AX		;  and save it
	mov	AX,P+2+SIZEPTR[BP]	;AX = pmode
	mov	-4[BP],AX

opens:
    if 1
	mov	AL,-2[BP]		;AL = rwmode (0,1,2)
	and	AL,0F3h			;clear reserved bits
	mov	AH,3Dh			;open file
    else
	mov	AX,3D00h		;see if file exists
    endif
	call	_bdos
	jc	nofile			;maybe doesn't exist
	mov	BX,AX			;opened file checking existance,
	bdos	3Eh			; now close it
	mov	BX,-2[BP]		;check for O_EXCL && O_CREAT
	and	BX,O_EXCL+O_CREAT
	cmp	BX,O_EXCL+O_CREAT
	jne	gotfile			;all's OK - continue
	mov	AX,EEXIST		;return EEXIST error

DOX:	stc
DOXX:	mov	SP,BP
	jmp	F2

nofile:
	.if	AX ne ENOENT, DOX	;if not "file not found" error
	test	word ptr -2[BP],O_CREAT	;check for O_CREAT
	jz	DOX			;no, return ENOENT
	clr	CX
	test	byte ptr -4[BP],S_IWRITE ;Write permission?
	jnz	DO2			;Yes, continue
	inc	CX			;No, set FA_RDONLY
DO2:
	mov	AH,3Ch			;create file
	call	_bdos
	jc	DOXX			;error

	test	byte ptr -4[BP],S_IWRITE ;Write permission?
	jz	DOXX			;no, return as it is

	mov	BX,AX
	bdos	3Eh			;file is created, now close it
	jc	DOXX
gotfile:
	mov	AL,-2[BP]		;AL = rwmode (0,1,2)
	and	AL,0F3h			;clear reserved bits
	mov	AH,3Dh			;open file
	call	_bdos
	jc	DOXX

	clr	CX
	mov	BX,AX			;BX = fd
	test	word ptr -2[BP],O_TRUNC	;Truncate?
	jz	notrunc			;No, skip it

	;Truncate it by writing 0 bytes to file
	bdos	40h			;write
	jc	DOXX
	jmps	noappend

notrunc:
	test	word ptr -2[BP],O_APPEND ;Append?
	jz	noappend		;No, skip it

	mov	DX,CX			;offset is 0L
	mov	AX,4202h		;use SEEK_END
	bdos
	jc	DOXX

noappend:
	mov	AX,BX			;return fd
	mov	SP,BP
	WINLEAVE
	ret
c_endp	open

func	sopen
	WINENTER
	sub	SP,4
	.if	_osmajor ge 3, SO1	;DOS 3+?
	mov	AX,EINVAL		;invalid operation for DOS 2
	jmp	DOX
SO1:
	mov	AX,P+SIZEPTR[BP]	;AX = rwmode (0,1,2)
	and	AL,8Fh
	mov	BX,P+2+SIZEPTR[BP]	;Add sharing bits...
	and	BL,70h
	or	AL,BL
	mov	-2[BP],AX		; and save it
	mov	AX,P+4+SIZEPTR[BP]	;AX = pmode
	mov	-4[BP],AX
	jmp	opens
c_endp	sopen

;;;;;;;;;;;;;;;;;;;;;;;;;
; Create a file
; Use:
;	int creat(name,pmode)
; Returns:
;	-1	error
;	fd	file handle
;

func	creat
	WINENTER
	mov	CL,P+SIZEPTR[BP]
	rol	CL,1			;put S_IWRITE in bit 0
	inc	CL			;toggle it
	and	CX,1			;turning it into read-only bit
	jmps	creat1
c_endp	creat

func	dos_creat
	WINENTER
	mov	CX,P+SIZEPTR[BP]	;CX = file attribute
creat1:
	mov	AH,3Ch			;create file
    if SPTR
	mov	DX,P[BP]		;DX -> name
	bdos
    else
	push	DS
	lds	DX,P[BP]		;DS:DX -> name
	bdos
	pop	DS
    endif
	jnc	creat4			;no error
	errno_set			;save DOS error number
	sbb	AX,AX			;error
creat4: WINLEAVE
	ret
c_endp	dos_creat

;;;;;;;;;;;;;;;;;;;;;;;;;
; Close a file
; Use:
;	int close(fd)
; Returns:
;	-1	error
;	0	successful
;

func	close
	WINENTER
	mov	BX,P[BP]		;file handle
	bdos	3Eh			;write to file or device
	jmp	U2
c_endp	close

;;;;;;;;;;;;;;;;;;;;;;;;
; Rename a file.		J.K.H. 2/15/86
; Use:
;	int rename (from, to)
; Returns:
;	-1	error
;	0	successful
;

func	rename
	WINENTER
	.save	DI
	mov	AH,56h			;DOS rename function
    if SPTR
      ife ESeqDS
	mov	BX,DS
	mov	ES,BX
      endif
	mov	DI,P+SIZEPTR[BP]	;The new name.
    else    ;LPTR
	les	DI,P+SIZEPTR[BP]	;The new name.
    endif
	jmps	U1
c_endp	rename

;;;;;;;;;;;;;;;;;;;;;;;;;
; Delete a file
; Use:
;	int remove(char *name)
; Returns:
;	-1	error
;	0	successful
;

func	remove
c_endp	remove

func	unlink
	WINENTER
	.save	DI
	mov	AH,41h			;delete file function
U1:
    if SPTR
	mov	DX,P[BP]		;DX -> name
	bdos
    else
	push	DS
	lds	DX,P[BP]		;DS:DX -> name
	bdos
	pop	DS
    endif
	.restore DI
U2:	WINLEAVE
	jnc	L7			;no error
	errno_set
L7:	sbb	AX,AX			;-1 if C, 0 if not
	ret
c_endp	unlink

;;;;;;;;;;;;;;;;;;;;;;;;;
; Seek to specified file position.
; Use:
;	long lseek(int fd,long offset,int mode)
; Input:
;	mode =	SEEK_SET or SEEK_CUR or SEEK_END
; Returns:
;	-1L	error
;	n	new file position
;

func	lseek
	WINENTER
	mov	BX,P[BP]		;file handle
	mov	DX,P+2[BP]		;lsw of offset
	mov	CX,P+4[BP]		;msw of offset
	mov	AL,P+6[BP]		;mode (0,1,2)
	bdos	42h			;write to file or device
	jnc	L9			;no error
	errno_set			;save DOS error number
	sbb	AX,AX			;AX = -1
	cwd				;DX = -1
L9:	WINLEAVE
	ret
c_endp	lseek

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get and return the size of a file.
; Use:
;	long filesize(filename)
;	char *filename;
; Returns:
;	-1L	error

func	filesize
	WINENTER
	push	DS
	sub	SP,44		;44 bytes for DTA

	;Set DTA to the 44 bytes on the stack (SS:BP)
    if LPTR
	push	SS
	pop	DS
    endif
	lea	DX,-44-2[BP]	;DS:DX is DTA
	bdos	1Ah		;set DTA

	;Find first matching file
    if SPTR
	mov	DX,P[BP]
    else
	lds	DX,P[BP]
    endif
	mov	CX,6		;find all normal files, plus system and hidden
	bdos	4Eh		;findfirst (DS:DX -> filename)
	jc	L11		;no error

	;Load file size from DTA
	mov	DX,-44-2+28[BP]
	mov	AX,-44-2+26[BP]

L12:	add	SP,44
	pop	DS
	WINLEAVE
	ret

L11:
    if LPTR
	mov	DS,-2[BP]	;restore DS
    endif
	errno_set		;remember error code
	sbb	AX,AX
	cwd			;return -1L on error
	jmp	L12
c_endp	filesize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Determine if handle is a tty.
; Use:
;	int isatty(fd)
;	int fd;
; Returns:
;	!=0	character device
;	0	not a character device or error

func	isatty
	WINENTER
	mov	BX,P[BP]
    ifdef _WINDOWS
	.if	BX ae 3, _I0	;stdin,stdout,stderr are handled by WINC spec
	  mov	  AH,0FFh	;force character device
	  jmp	  short _I1
_I0:
    endif
	mov	AX,04400h	;get device information
	bdos			;IOCTL
	cmc
	jnc	I4		;error

	;If sign bit is set in DL, it is a character device.
	mov	AL,DL
	cbw			;AH = 0FFh if char dev, else 0
_I1:	mov	AL,AH
	WINLEAVE
	ret
c_endp	isatty

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	#include <io.h>
;	int access(char *filename,int mode);
; Synopsis:
;	Test file for existence and it's access bits. The access bits are:
;	#define F_OK	0	/* does file exist?	*/
;	#define X_OK	1	/* execute permission?	*/
;	#define W_OK	2	/* write permission?	*/
;	#define R_OK	4	/* read permission?	*/
;	OR these values together to test a combination.
; Bugs:
;	Since MS-DOS only records a write permission bit, the X_OK and
;	R_OK access permissions always succeed. They are included here
;	for unix compatibility.
; Returns:
;	0	file exists and access mode is granted
;	-1	otherwise and errno is set

	c_public access
func	access
	WINENTER
	mov	AX,4300h	;get file's attribute
    if LPTR
	push	DS
	lds	DX,P[BP]			;DS:DX = path
	bdos
	pop	DS
	jc	I4
	test	byte ptr P+SIZEPTR[BP],2	;is W_OK set?
    else
	mov	DX,P[BP]			;DS:DX = path
	bdos
	jc	I4
	test	byte ptr P+SIZEPTR[BP],2	;is W_OK set?
    endif
	jz	I7				;no
	shr	CL,1				;is file read-only?
	jnc	I7				;no
	mov	AX,EACCES
I4:	errno_set
I7:	sbb	AX,AX				;-1 if C, 0 if not
	WINLEAVE
	ret
c_endp	access

    endif ;__OS2__

	endcode	io

	end
