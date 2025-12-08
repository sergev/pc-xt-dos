;;;;;;;;;;;;;;;;;;;;;;;;;
; C compiler system calls for Phar Lap DOS386.
;Copyright (C) 1985-1990 by Walter Bright
;All Rights Reserved
;
; Modified by Joe Huffman September 17, 1990

include macros.asm

;Open flags
O_RDONLY	equ	0
O_WRONLY	equ	1
O_RDWR		equ	2
O_APPEND	equ	8
O_NOINHERIT	equ	80h
O_CREAT		equ	100h
O_TRUNC		equ	200h
O_EXCL		equ	400h

;Error numbers
ENOENT		equ	2
EINVAL		equ	22
EEXIST		equ	80

;Permission modes
S_IWRITE	equ	80h
S_IREAD		equ	0100h

	begdata
	c_extrn errno,dword
	c_extrn _osmajor,byte
	enddata

	begcode io

	c_public read,write,open,sopen,close,creat
	c_public lseek,filesize,rename,unlink

	c_public isatty,remove,dos_open,dos_creat

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
F4:	push	EBP
	mov	EBP,ESP
	uses	<EBX,ECX,EDX>
	mov	EBX,P[EBP]		;EBX = fd (file handle)
	mov	EDX,P+4[EBP]		;EDX = buffer address
	mov	ECX,P+4+SIZEPTR[EBP]	;ECX = number of bytes to read/write
	bdos				;read/write from/to file or device
	unuse	<EDX,ECX,EBX>

F2:	jnc	F1			;no error
	movsx	EAX,AX
	mov	errno,EAX		;save DOS error number
	sbb	EAX,EAX			;error

F1:	pop	EBP
	ret
c_endp	read


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
; Open a file (DOS mode)
; Use:
;	int dos_open(name,rwmode)
; Returns:
;	-1	error
;	fd	file handle
;

func	dos_open
	push	EBP
	mov	EBP,ESP
	mov	AH,3Dh			;open file
	mov	AL,P+SIZEPTR[EBP]	;AL = rwmode (0,1,2)
OPEN1:
	mov	EDX,P[EBP]		 ;EDX -> name
	bdos
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

rwmode_data	equ	-4[EBP]
pmode_data	equ	-8[EBP]

func	open
	push	EBP
	mov	EBP,ESP
	sub	ESP,8
	uses	<EBX>
	mov	EAX,P+SIZEPTR[EBP]	;EAX = rwmode (0,1,2)
	and	AL,8Fh			;Mask off sharing bits...
;	or	AL,40h			; add sharing (deny none)...
	_if	_osmajor ge 3, DO4	;if DOS 3+?
	and	AL,7			;only these bits have meaning
DO4:	mov	rwmode_data,EAX		;  and save it
	mov	EAX,P+4+SIZEPTR[EBP]	;EAX = pmode
	mov	pmode_data,EAX

opens:
    if 1
	mov	AL,rwmode_data		;AL = rwmode (0,1,2)
	and	AL,0F3h			;clear reserved bits
	mov	AH,3Dh			;open file
    else
	mov	AX,3D00h		;see if file exists
    endif
	mov	EDX,P[EBP]		;filename offset
	bdos
	jc	nofile			;maybe doesn't exist

	mov	EBX,EAX			;opened file checking existance,
	bdos	3Eh			; now close it
	mov	EBX,rwmode_data		;check for O_EXCL && O_CREAT
	and	EBX,O_EXCL+O_CREAT
	cmp	EBX,O_EXCL+O_CREAT

	jne	gotfile			;all's OK - continue
	mov	EAX,EEXIST		;return EEXIST error

DOX:	stc
DOXX:	unuse	<EBX>
	mov	ESP,EBP
	jmp	F2

nofile:
	_ifs	AX ne ENOENT, DOX	;if not "file not found" error
	test	word ptr rwmode_data,O_CREAT ;check for O_CREAT
	jz	DOX			;no, return ENOENT
	clr	ECX
	test	byte ptr pmode_data,S_IWRITE ;Write permission?
	jnz	DO2			;Yes, continue
	inc	ECX			;No, set FA_RDONLY
DO2:
	mov	EDX,P[EBP]		;EDX -> name
	bdos	3ch			;create file
	jc	DOXX			;error

	test	byte ptr rwmode_data,S_IWRITE ;Write permission?
	jz	DOXX			;no, return as it is

	mov	EBX,EAX
	bdos	3Eh			;file is created, now close it

	jc	DOXX
gotfile:
	mov	AL,rwmode_data		;AL = rwmode (0,1,2)
	and	AL,0F3h
	mov	AH,3Dh			;open file

	mov	EDX,P[EBP]		;EDX -> name
	bdos
	jc	DOXX

	clr	ECX
	movzx	EBX,AX			;fd
	test	word ptr rwmode_data,O_TRUNC ;Truncate?
	jz	notrunc			;No, skip it

	;Truncate it by writing 0 bytes to file
	bdos	40h			;write
	jc	DOXX
	jmps	noappend

notrunc:
	test	word ptr rwmode_data,O_APPEND ;Append?
	jz	noappend		;No, skip it

	mov	EDX,ECX			;offset is 0L
	mov	AX,4202h		;use SEEK_END
	bdos
	jc	DOXX

noappend:
	movzx	EAX,BX			;return fd
	unuse	<EBX>
	mov	ESP,EBP
	pop	EBP
	ret
c_endp	open

func	sopen
	push	EBP
	mov	EBP,ESP
	sub	ESP,8
	uses	<EBX>

	_if	_osmajor ge 3, SO1	;DOS 3+?
	mov	EAX,EINVAL
	jmp	DOX
SO1:
	mov	EAX,P+SIZEPTR[EBP]	;EAX = rwmode (0,1,2)
	and	AL,8fh
	mov	ECX,P+4+SIZEPTR[EBP]	;Add sharing bits...
	and	CL,70h
	or	AL,CL
	mov	-4[EBP],AX		; and save it
	mov	EAX,P+8+SIZEPTR[EBP]	;EAX = pmode
	mov	-8[EBP],EAX
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
	push	EBP
	mov	EBP,ESP
	movzx	ECX,byte ptr P+SIZEPTR[EBP]
	rol	CL,1			;put S_IWRITE in bit 0
	inc	CL			;toggle it
	and	ECX,1			;turning it into read-only bit
	jmps	creat1
c_endp	creat

func	dos_creat
	push	EBP
	mov	EBP,ESP
	mov	ECX,P+SIZEPTR[EBP]	;ECX = file attribute
creat1:
	mov	AH,3Ch			;create file
	mov	EDX,P[EBP]		;EDX -> name
	bdos
	movzx	EAX,AX

	jnc	short creat4		;no error
	mov	errno,EAX		;save DOS error number
	sbb	EAX,EAX			;error
creat4: pop	EBP
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
	push	EBP
	mov	EBP,ESP
	uses	<EBX>
	mov	EBX,P[EBP]		;file handle
	bdos	3Eh			;write to file or device
	unuse	<EBX>
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
	push	EBP
	mov	EBP,ESP
	uses	<EDI>
      ife ESeqDS
	mov	AX,DS
	mov	ES,AX
      endif
	mov	AH,56h			;DOS rename function
	mov	EDI,P+SIZEPTR[EBP]	;The new name.
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
	push	EBP
	mov	EBP,ESP
	uses	<EDI>
	mov	AH,41h			;delete file function
U1:
	mov	EDX,P[EBP]		;EDX -> name
	bdos
	unuse	<EDI>
U2:	pop	EBP
	jnc	L7			;no error
L8:	movsx	EAX,AX
	mov	errno,EAX
L7:	sbb	EAX,EAX			;-1 if C, 0 if not
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
	push	EBP
	mov	EBP,ESP
	uses	<EBX,ECX,EDX>
	mov	EBX,P[EBP]		;file handle
	mov	DX,P+4[EBP]		;lsw of offset
	mov	CX,P+6[EBP]		;msw of offset
	mov	AL,P+8[EBP]		;mode (0,1,2)
	bdos	42h			;write to file or device
	movzx	EAX,AX
	jnc	short L9		;no error
	mov	errno,EAX		;save DOS error number
	sbb	EAX,EAX			;EAX = -1
L9:	shl	EDX,16			;mov msw to top half of reg.
	or	EAX,EDX			;combine into one reg.
	unuse	<EDX,ECX,EBX>
	pop	EBP
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
	push	EBP
	mov	EBP,ESP
	sub	ESP,44		;44 bytes for DTA

	;Set DTA to the 44 bytes on the stack (SS:EBP)
	mov	EDX,ESP		;DS:EDX is DTA
	bdos	1Ah		;set DTA

	;Find first matching file
	mov	EDX,P[EBP]
	mov	CX,6		;find all normal files, plus system and hidden
	bdos	4Eh		;findfirst (DS:EDX -> filename)
	jc	L11		;no error

	;Load file size from DTA
	mov	EAX,26[ESP]

L12:	leave
	ret

L11:
	movsx	EAX,AX
	mov	errno,EAX	;remember error code
	sbb	EAX,EAX
	cwd			;return -1L on error
	leave
	ret
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
	push	EBX
	mov	EBX,P[ESP]	;get fd (file handle)
	mov	AX,04400h	;get device information
	bdos			;IOCTL
	pop	EBX
	cmc
	jnc	I4		;error

	;If sign bit is set in DL, it is a character device.
	movsx	EAX,DL		;AH = 0FFh if char dev, else 0
	mov	AL,AH
	ret
c_endp	isatty

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	#include <io.h>
;	int access(char *path,int mode);
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
	mov	AX,4300h	;get file's attribute
	mov	EDX,PS[ESP]	;DS:DX = path
	bdos
	jc	I4
	test	byte ptr PS+SIZEPTR[ESP],2	;is W_OK set?
	jz	L7				;no
	shr	CL,1		;is file read-only?
	jnc	L7		;no
	mov	EAX,5		;EACCES
I4:	jmp	L8
c_endp	access

	endcode io

	end

