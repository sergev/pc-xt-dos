;_ alloc.asm   Sun Apr	8 1990	 Modified by: Walter Bright
; Windows support added by G. Eric Engstrom Dec 1990,	-D_WINDOWS
; RATIONAL support added by P. Murray April 1990,	-DDOS16RM
; OS2 support added by Nikki Locke May 1989,		-D__OS2__
; Copyright (C) 1985-1991 by Walter Bright
; All rights reserved
; Written by Walter Bright

include	macros.asm

;;;;;;;;;;;;;;;;;;;;;;;;;;
; Do far pointer normalization
; SCRATCH is a scratch register we can destroy

    ifdef DOS16RM
    PROTECTED EQU 1
    endif
    ifdef _WINDOWS
    PROTECTED EQU 1
    endif

    ifdef __OS2__
normptr	macro	MSREG,LSREG,SCRATCH
	endm
    else ;__OS2__
    ifdef PROTECTED
normptr macro	MSREG,LSREG,SCRATCH
	endm
    else ;PROTECTED
normptr	macro	MSREG,LSREG,SCRATCH
	mov	SCRATCH,LSREG
	and	LSREG,0Fh
	shr	SCRATCH,1
	shr	SCRATCH,1
	shr	SCRATCH,1
	shr	SCRATCH,1
	add	MSREG,SCRATCH
	endm
    endif ;PROTECTED
    endif ;__OS2__

    ifdef __OS2__
extrn	DOSWRITE:far
extrn	DOSEXIT:far
    endif

    if LCODE
	c_extrn	sbrk,far
	extrn	___faterr:far
    else
	c_extrn	sbrk,near
	extrn	___faterr:near
    endif

    ifdef _MT
    if LCODE
	extrn	__semerr:far
    else
	extrn	__semerr:near
    endif
    endif

	begcode	alloc

	c_public malloc,calloc,realloc,free
	public	___mallocf,___freef

; Storage allocator

	begdata
	c_public _baslnk
    ifdef DOS16RM
	c_public _almblksiz
    endif

    if SPTR
	c_extrn	_pastdata,word, _heapbottom,word
    endif

    if SPTR
__baslnk	dw	offset DGROUP:__baslnk	;starting link for
						; storage allocator
		dw	0	;give it a size of 0 so it is never allocated
__allocp	dw	offset DGROUP:__baslnk	;roving pointer for allocator
    else

__baslnk	dw	offset DGROUP:__baslnk
		dw	seg DGROUP:__baslnk
		dw	0

__allocp	dw	-1,?
    ifdef DOS16RM
__almblksiz	dw	1000h		; Minimum block allocation size.
    endif
    endif
_allocp	equ	__allocp
_baslnk equ	__baslnk

    ifdef _WINDOWS
heapmsg		db	'Heap is corrupted',0
    else
heapmsg		db	0Dh,0Ah,'Heap is corrupted',0Dh,0Ah,0
    endif

	enddata

; A block in the free list consists of:
;	dw	pointer to next block in list
;	dw	segment of next block in list (for LPTR)
;	dw	size of block in bytes (must be even) (including both words)

; When it's allocated,
;	dw	# of bytes in this block including this word
;	db...	the bytes allocated

.if32	macro	r1H,r1L,b,r2H,r2L,lbl
	local	L1
	.if	r1H ne r2H, L1
	cmp	r1L,r2L
L1:	j&b	lbl
	endm

mov32	macro	ah,al,bh,bl
	mov	ah,bh
	mov	al,bl
	endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Handle locking/unlocking for multithreaded programs

    ifdef _MT			;if multi-threaded library support

;extrn	DOSENTERCRITSEC:far
;extrn	DOSEXITCRITSEC:far
extrn	DOSSEMREQUEST:far
extrn	DOSSEMCLEAR:far

talloc_lock macro	RX
	local	L1
    ifnb <RX>
	push	RX		;save register
    endif
    if 0
	call	DOSENTERCRITSEC
    else
;	mov	AX,DGROUP
;	push	AX
	push	DS
	mov	AX,offset DGROUP:alloc_sem
	push	AX
	mov	AX,-1
	push	AX
	push	AX		;wait until semaphore is unowned
	call	DOSSEMREQUEST
	tst	AX
	jz	L1
    if LCODE
	jmp	semerr
    else
	jmp	__semerr
    endif
L1:
    endif
    ifnb <RX>
	pop	RX
    endif
	endm

talloc_unlock macro	RX
	local	L1
    ifnb <RX>
	push	RX		;save register
    endif
    if 0
	call	DOSEXITCRITSEC
    else
;	mov	AX,DGROUP
;	push	AX
	push	DS
	mov	AX,offset DGROUP:alloc_sem
	push	AX
	call	DOSSEMCLEAR
	tst	AX
	jz	L1
    if LCODE
	jmp	semerr
    else
	jmp	__semerr
    endif
L1:
    endif
    ifnb <RX>
	pop	RX
    endif
	endm

	begdata

alloc_sem	dd	0	;ram semaphore
		dd	0
	enddata

    else
talloc_lock macro
	endm

talloc_unlock macro RX
	endm
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; When out of memory, this function pointer is called in an attempt to get more.
; The function pointer can be set by the application to point at a function
; that can, say, free up some disk buffers.
; Returns:
;	0	unsuccessful at finding more memory to free
;	!=0	found some, try allocating it

	ifndef __OS2__

func	default_malloc_handler
	clr	AX		;didn't find any
	ret
c_endp	default_malloc_handler

	begdata

	public	__malloc_handler

    if LCODE
__malloc_handler	dd	default_malloc_handler
    else
__malloc_handler	dw	offset default_malloc_handler
    endif

	enddata

	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Something went wrong with the semaphores.
; Print an error message and abort the program.

    ifdef _MT
    if LCODE
semerr	proc	near
	jmp	__semerr
semerr	endp
    endif
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; The heap has been corrupted.
; Print an error message and abort the program.

heaperr	proc	near
	mov	AX,offset DGROUP:heapmsg
	jmp	___faterr
heaperr	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data and clear it.
; Use:
;	p = calloc(numelems,sizeof(elem));
; Returns:
;	pointer to allocated data else NULL

	even
func	calloc
	WINENTER_VCM
	mov	AX,P[BP]	;get numelems
	mov	BX,P+2[BP]	;get sizeof(elem)
	.if	BX e 1, C1	;no need to multiply
	mul	BX
	jc	C3		;if overflow
C1:	push	AX		;nbytes
	callm	malloc
	mov	SP,BP
    if SPTR
	tst	AX
    else
	tst	DX
    endif
	jz	C2		;yes
	.save	<DI>
    if SPTR
      ife ESeqDS
	mov	DX,DS
	mov	ES,DX
      endif
	mov	DI,AX
	mov	DX,AX		;save pointer to result
	mov	CX,-2[DI]	;# of bytes
    else
	mov	ES,DX
	mov	DI,AX
	mov	BX,AX
	mov	CX,ES:-2[DI]
    endif
	shr	CX,1		;# of words (including byte count)
    ifdef DOS16RM
	jne	pp1		;if not 64k bytes
	mov	CH,080h		;32k words
pp1:
    endif
	dec	CX		;skip # of bytes
	clr	AX
	rep	stosw		;clear the memory
    if SPTR
	mov	AX,DX		;restore pointer to result
    else
	mov	AX,BX		;DX:AX is pointer to result
    endif
	.restore <DI>
C2:	WINLEAVE_VCM
	ret

C3:	clr	AX
    if LPTR
	cwd
    endif
	WINLEAVE_VCM
	ret
c_endp	calloc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Entry point to malloc that is always accessed by a far call,
; and always returns a far pointer.

    ifdef I8086V
    else
___mallocf proc far
	mov	BX,SP
    ife SSeqDS
	push	SS:4[BX]
    else
	push	4[BX]
    endif
	callm	malloc
	pop	BX
    if SPTR
	;Convert to far pointer
	cwd
	tst	AX
	jz	MF1		;return 0:0
	mov	DX,DS		;segment of heap
MF1:
    endif
	ret
___mallocf endp
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data.
; Use:
;	char *malloc();
;	p = malloc(nbytes);
; Returns:
;	pointer to allocated data else NULL

	even
    if SPTR
    ifdef _WINDOWS
func	malloc
	WINENTER
	.save	<SI,DI>
A4:
	mov	AX,P[BP]	;get nbytes
	add	AX,3		;need another word for length info
	and	AX,0FFFEh	;round up to nearest word
	.if	AX b 4, allocerr	;can't allocate 0 bytes

	mov	SI,_baslnk	;last item
	mov	CX,SI		;CX to save bytes
	jmps	A2

	even
A1:	mov	SI,DI
	.if	SI e CX, trysbrk	;wrapped around, didn't find any
A2:	mov	DI,[SI]			;next item in list
	.if	AX a [DI+2], A1	;not big enough

	je	A3		;exactly big enough
	inc	AX		;we'll need another 2 bytes
        inc	AX
	.if	AX e [DI+2],A3	;have to allocate an entire block
	dec	AX
        dec	AX

;Allocate from bottom of free block. Desirable in order to delay
;stack overflow as long as possible.
; DI -> free block
; SI -> previous free block
; AX =	# of bytes in allocated block

	add	[SI],AX		;link to new free block
	mov	SI,[SI]		;pointer to new free block
	mov	CX,[DI+2]	;number of bytes in block we're splitting
	sub	CX,AX		;CX = remaining bytes
	mov	[SI+2],CX	;# of bytes in this block

A3:	xchg	AX,[DI]		;[DI] = # of bytes, AX = next free block
	mov	[SI],AX		;skip the DI entry in list
	mov	_allocp,SI
	lea	AX,[DI+2]	;pointer to area allocated (DI + 2)
A6:	.restore <DI,SI>
	WINLEAVE
	ret

trysbrk:			;try sbrk() to grow our data segment
	.if	AX ae 1024, A5
	mov	AX,1024		;1024 byte chunk minimum size
A5:
	push	AX
	callm	sbrk
	pop	BX
	.if	AX e -1, allocerr	;failed
	inc	AX		;point past # of bytes allocated
	inc	AX
	push	AX
	callm	free		;add allocated memory into free list
	pop	BX
	jmp	A4		;try again

allocerr:
	clr	AX		;NULL
	jmp	A6
c_endp	malloc
    else
func	malloc
	talloc_lock
	push	BP
A4:	mov	BP,SP
	.save	<SI,DI>
	mov	AX,P[BP]	;get nbytes
	add	AX,3		;need another word for length info
	and	AL,0FEh		;round up to nearest word
	.if	AX b 4, allocerr	;can't allocate 0 bytes

	mov	BP,2		;save some bytes
;	mov	SI,_allocp	;last item
	mov	SI,_baslnk	;last item
	mov	CX,SI		;CX to save bytes
	jmps	A2

allocerr:
	clr	AX		;NULL
	jmp	A6

	even
A1:	mov	SI,DI
	.if	SI e CX, trysbrk	;wrapped around, didn't find any
A2:	mov	DI,[SI]			;next item in list
	.if	AX a [DI+BP], A1	;not big enough

	je	A3		;exactly big enough
	add	AX,BP		;we'll need another 2 bytes
	.if	AX e [DI+BP],A3	;have to allocate an entire block
	sub	AX,BP

;Allocate from bottom of free block. Desirable in order to delay
;stack overflow as long as possible.
; DI -> free block
; SI -> previous free block
; AX =	# of bytes in allocated block
	add	[SI],AX		;link to new free block
	mov	SI,[SI]		;pointer to new free block
	mov	CX,[DI+BP]	;number of bytes in block we're splitting
	sub	CX,AX		;CX = remaining bytes
	mov	[SI+BP],CX	;# of bytes in this block

A3:	xchg	AX,[DI]		;[DI] = # of bytes, AX = next free block
	mov	[SI],AX		;skip the DI entry in list
	mov	_allocp,SI
	lea	AX,[DI+BP]	;pointer to area allocated (DI + 2)
A6:	talloc_unlock AX
	.restore <DI,SI>
	WINLEAVE
	ret

trysbrk:			;try sbrk() to grow our data segment
    ifdef __OS2__
	.if	AX ae 1024, A5
	mov	AX,1024		;1024 byte chunk minimum size
    else ;__OS2__
	.if	AX ae 256, A5
	mov	AX,256		; 256 byte chunk minimum size
    endif ;__OS2__
A5:
	push	AX
	callm	sbrk
	pop	BX
	inc	AX		;point past # of bytes allocated
	jz	A6		;failed, sbrk returned -1, so return NULL
	inc	AX
	push	AX
	callm	free_nlock	;add allocated memory into free list
	talloc_lock
	pop	BX
	.restore <DI,SI>
	jmp	A4		;try again
c_endp	malloc
    endif ;_WINDOWS
    else ;LPTR
;;;;;;;;;;;;;;;;;;;;;;;;
; malloc() for large data models

A17:	mov	AX,6			;minimum allocation size
	jmp	short A5

A16:	;Initialize
	mov32	AX,BX _baslnk+2,_baslnk
    ifndef PROTECTED
    ifndef __OS2__
	normptr	AX,BX, CX		;normalize _baslnk
	mov32	_baslnk+2,_baslnk AX,BX
    endif
    endif
	mov32	_allocp+2,_allocp AX,BX
	talloc_unlock

	even
    ifdef I8086V
___mallocf:
    endif
func	malloc
	talloc_lock
	.if	_allocp e -1, A16	;if not initialized
	WINENTER_VCM
	.save	<SI,DI>
	push	DS
;	nbytes = (nbytes + 3) / 4 * 4
A4:	mov	AX,P[BP]
	add	AX,3
	and	AL,0FEh
;	if (nbytes < 4)
;		return 0
	.if	AX b 4, mallocerr
;	if (nbytes < 6)
;		nbytes = 6
	.if	AX b 6, A17
A5:
	mov32	DX,DI _baslnk+2,_baslnk		;pstart = _baslnk
	mov32	CX,SI DX,DI			;p = pstart
	mov	DS,CX
;	loop
	even
A7:
	mov	BX,[SI]
	mov	DS,2[SI]		;pnext = p->next

	.if	AX be 4[BX], A6		;if (nbytes <= pnext->size)

	mov	CX,DS
	mov	SI,BX			;p = pnext
	.if	CX ne DX, A7
	.if	SI ne DI, A7		;if (p != pstart)

	;We've wrapped around, meaning no block is big enough
;			p = wsbrk(nbytes)
	mov	DS,-6[BP]
    ifdef __OS2__
	.if	AH ae 10h, A14
	mov	AX,4096		;4096 minimum growth size
    else ;__OS2__
    ifdef DOS16RM
	.if	AX a _almblksiz, A14	;allocate a new allocation
					;block from the system. Clamp min
	mov	AX, _almblksiz		;allocation size to 4K bytes.
	add	AX, 2		;for our overhead
	jc	mallocerr	;64K allocations are illegal
    else ;DOS16RM
    ifdef _WINDOWS
	.if	AH ae 80h, A14
	mov	AX,32768	;32768 minimum growth size SCC need to try successively smaller chunks if this doesn't work
    else ;_WINDOWS
	.if	AH ae 2, A14
	mov	AX,512		;512 minimum growth size
    endif ;_WINDOWS
    endif ;DOS16RM
    endif ;__OS2__
A14:	push	AX
	callm	sbrk		;extend program segment
	pop	BX		;fix stack
;			if (p == -1)
;				return 0
	inc	AX
	jz	A15		;error (p == -1)
	inc	AX
	push	DX
	push	AX
	callm	free_nlock	;add allocated memory into free list
	talloc_lock
	pop	AX
	pop	AX
	jmp	A4
A15:
    ifndef __OS2__
	;Try calling __malloc_handler to see if it can
	;find any memory

	talloc_unlock
	call	__malloc_handler	; SCC HERE WE PUT RETRY FOR JUST THE AMOUNT HE NEEDS
	talloc_lock AX
	tst	AX
	jnz	A4		;found some, try again
    endif
mallocerr:
	clr	AX
	cwd
	pop	DS
	jmp	A11

A6:	; We have:
	;	pnext -> block to alloc
	;	pnext = DS:BX

;	if (nbytes + sizeof(*pnext) > pnext->size)
	mov	DX,AX
	add	DX,6
	jc	A10
	.if	DX be 4[BX], A9

;		Allocate entire block that pnext points to.
A10:	mov32	DX,DI 2[BX],[BX]
	mov	ES,CX
	mov32	ES:2[SI],ES:[SI] DX,DI	;p->next = pnext->next

	mov	DX,4[BX]
	mov	[BX],DX			;*pnext = pnext->size

	jmp	A8

A9:
; Create new block pnew that consists of the remainder of pnext.
; At this point, we have:
;	AX = nbytes
;	CX:SI = p
;	DS:BX = pnext

;	pnew = pnext + nbytes

	mov	DI,BX
	add	DI,AX			;no overflow possible here
	mov	DX,DS
	normptr	DX,DI, BP		;DX:DI = pnew

	push	AX
	neg	AX
	add	AX,4[BX]
	mov	ES,DX
	mov	ES:4[DI],AX		;pnew->size = pnext->size - nbytes

	mov	AX,[BX]
	mov	ES:[DI],AX
	mov	AX,2[BX]
	mov	ES:2[DI],AX		;pnew->next = pnext->next

	mov	ES,CX
	mov32	ES:2[SI],ES:[SI] DX,DI	;p->next = pnew

	pop	[BX]			;*pnext = nbytes

A8:	;return pnext + 2
	; DS:BX = pnext
	mov	DX,DS
	mov	AX,BX
	inc	AX
	inc	AX

	pop	DS
	mov32	_allocp+2,_allocp CX,SI			;_allocp = p
A11:	.restore <DI,SI>
	WINLEAVE_VCM
	talloc_unlock AX
	ret
c_endp	malloc
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Reallocate memory that was allocated by malloc() or calloc().
; Use:
;	char *realloc(char *p, unsigned nbytes)
; Returns:
;	0 error
;	else pointer to reallocated memory

R6:	WINLEAVE_VCM
	jmp	near ptr free		;free(p)

func	realloc
	WINENTER_VCM

	mov	AX,P+SIZEPTR[BP]	;AX = nbytes
	tst	AX			;trying to realloc() to 0 size?
	jz	R6			;yes

	;if realloced size is smaller, attempt to just shrink current block
    if SPTR
	mov	BX,P[BP]	;BX = p
	tst	BX		;is p NULL?
	jnz	R5		;no
	;function just like malloc(nbytes)
	push	AX
	callm	malloc
        mov	SP,BP
        WINLEAVE_VCM
	ret

R5:	dec	BX
	dec	BX
	mov	CX,[BX]		;CX = # of bytes in this block
	add	AX,3
	jc	rallocerr	;overflow
	and	AL,0FEh		;AX = real new size
    else
	mov	BX,P[BP]	;ES:BX = p
	mov	CX,P+2[BP]
	or	CX,BX		;is p NULL?
	jnz	R5		;no
	;function just like malloc(nbytes)
	push	AX
	callm	malloc
	mov	SP,BP
	WINLEAVE_VCM
	ret

R8:	jmp	rallocerr

R5:	mov	ES,P+2[BP]
	dec	BX
	dec	BX
	mov	CX,ES:[BX]
	add	AX,3
	jc	R8		;overflow
	and	AL,0FEh		;AX = real new size

	;Make sure that we don't create an allocated unit that's less than
	;6 bytes.
	.if	AX ae <SIZEPTR+2>, R7
	mov	AL,SIZEPTR+2
R7:
    endif
	sub	CX,AX
	jb	R3			;if allocating more bytes
	.if	CX b <SIZEPTR+2>, R4	;size of free list entry
    if SPTR
	mov	[BX],AX		;realloced size of p
	add	BX,AX
	mov	[BX],CX		;size of new fragment
	inc	BX
	inc	BX
	push	BX
	callm	free
	pop	BX
    else
	.save	<DI>
	mov	DI,BX
	add	DI,AX
	mov	ES:[DI],CX	;size of new fragment
	mov	ES:[BX],AX	;realloced size of p
	mov	BX,DI
	mov	AX,ES		;AX:BX is pointer to new fragment
	normptr	AX,BX, CX	;normalize it
	inc	BX		;point past size of fragment
	inc	BX
	push	AX
	push	BX
	callm	free
	add	SP,SIZEPTR
	.restore <DI>
    endif
R4:
    if SPTR
	mov	AX,P[BP]
    else
	mov32	DX,AX P+2[BP],P[BP]	;reload original pointer p
    endif
	jmps	R1		;no change, return p

	;we'll have to allocate a new block, and free the old one
R3:
	push	P+SIZEPTR[BP]
	callm	malloc		;malloc(nbytes)
	mov	SP,BP
    if LPTR
	tst	DX
    else
	tst	AX
    endif
	jz	rallocerr	;error
	push	AX		;save pointer to new memory
	.save	<SI,DI>
    if SPTR
	mov	SI,P[BP]	;DS:SI -> original
      ife ESeqDS
	mov	CX,DS
	mov	ES,CX
      endif
	mov	DI,AX		;ES:DI -> new item
	mov	CX,-2[SI]
	.if	CX be -2[DI], R2
	mov	CX,-2[DI]	;CX = smaller of two size
R2:	shr	CX,1		;# of words
	dec	CX		;compensate for extra word in beginning
	rep	movsw		;transfer the words
	push	P[BP]
	callm	free		;free the old one
	add	SP,SIZEPTR
    else
	push	DX
	push	DS
	lds	SI,P[BP]	;DS:SI -> original
	mov	ES,DX
	mov	DI,AX		;ES:DI -> new item
	mov	CX,-2[SI]
	.if	CX be ES:-2[DI], R2
	mov	CX,ES:-2[DI]	;CX = smaller of two sizes
R2:	shr	CX,1		;# of words
	dec	CX		;compensate for extra word in beginning
	rep	movsw		;transfer the words
	pop	DS
	push	P+2[BP]
	push	P[BP]
	callm	free		;free the old one
	add	SP,SIZEPTR
	pop	DX
    endif
	.restore <DI,SI>
	pop	AX		;restore pointer to new memory
	jmp	short R1

rallocerr:
	clr	AX
    if LPTR
	cwd
    endif
R1:	WINLEAVE_VCM
	ret
c_endp	realloc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Entry point to free that is always accessed by a far call.
; The arg to ___free is always a far pointer.

___freef proc far
    if LCODE
	;fall through to free()
    else
	mov	BX,SP
    ife SSeqDS
      if LPTR
	push	SS:6[BX]	;segment
	push	SS:4[BX]	;offset
      else
	push	SS:4[BX]	;ignore segment (it should be 0 or DS)
      endif
    else
      if LPTR
	push	6[BX]
	push	4[BX]
      else
	push	4[BX]
      endif
    endif
	callm	free
	add	SP,SIZEPTR
	ret
    endif
___freef endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Free memory that was allocated by malloc() or calloc().
; Use:
;	free(p);

    if SPTR
func	free
	talloc_lock

_free_nlock:
	WINENTER
	.save	<SI,DI>
	mov	BX,P[BP]		;get p
	tst	BX			;pass a NULL pointer?
	jz	F5			;yes, return 0
	mov	BP,2			;to save some bytes
					;check if below bottom of pool
	.if	BX be _heapbottom, freeerr	;if below bottom of pool
	.if	BX ae _pastdata, freeerr	;if above top of pool
	test	BL,1			;odd?
	jne	freeerr
	sub	BX,BP			;point to start of block
	mov	AX,[BX]			;# of bytes in block to be freed

; Try to find SI and DI such that SI < BX < DI

	mov	SI,_allocp		;try our roving pointer
	.if	SI b BX, F1		;a good starting point
	mov	SI, offset DGROUP:_baslnk
	jmps	F1

	even
F6:	mov	SI,DI
F1:	mov	DI,[SI]			;the next in the list
	.if	SI ae BX, freeerr
	.if	DI a BX, F2		;got it
	.if	DI a SI, F6		;no wrap around (SI < DI < BX)

; We have SI < BX < DI (relative position in list)

F2:	mov	CX,[SI+BP]		;# of bytes in previous block
	add	CX,SI			;+ link
	.if	CX ne BX, F3		;if can't collapse with prev block
	add	[SI+BP],AX
	jmps	F4

F3:	mov	2[BX],AX		;store # of bytes in freed block
	mov	[BX],DI			;link to block after BX
	mov	[SI],BX			;link to BX
	mov	SI,BX

; See if we can collapse SI with DI
; SI -> block just before DI
; DI -> block just after SI
; BP = 2

F4:	mov	_allocp,SI		;for next time
	mov	AX,[SI+BP]
	add	AX,SI
	.if	AX ne DI, F5		;nope
	mov	AX,[DI]			;link after DI
	mov	[SI],AX			;becomes link after SI
	mov	AX,[DI+BP]		;# of bytes in DI
	add	[SI+BP],AX		;add to # of bytes in SI

F5:	talloc_unlock
	clr	AX			;success
	.restore <DI,SI>
	WINLEAVE
	ret

freeerr:
	jmp	heaperr
c_endp	free

    else ;LPTR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; free() for large data models

func	free
	talloc_lock

_free_nlock:
	WINENTER
	.save	<SI,DI>
	push	DS

	mov32	DX,DI P+2[BP],P[BP]	;DX:DI = pfree
	mov	AX,DX
	or	AX,DI			;if (pfree == NULL)
	jz	F9			;return 0

	test	DI,1			;odd pointers are errors
	jne	F8

	dec	DI
	dec	DI			;pfree -= 2

;	p = (_allocp < pfree) ? _allocp : &_baslnk
	mov32	CX,SI _allocp+2,_allocp		;CX:SI = _allocp
	.if32	CX,SI b DX,DI, F1
	mov32	CX,SI <seg DGROUP:_baslnk>,<offset DGROUP:_baslnk>
	jmp	short F1

F9:	jmp	F4
F8:	jmp	freeerr

;	loop
	even

F10:
		.if32 AX,BX be CX,SI, F2	;if (pnext <= p) break
		mov32	CX,SI AX,BX		;p = pnext

F1:		mov	ES,CX			;ES:SI = p
		mov32	AX,BX ES:2[SI],ES:[SI]	;pnext = p->next

;		.if32 CX,SI ae DX,DI, F8	;if (p >= pfree) then error

		.if	AX b DX, F10
		ja	F2
		.if	BX be DI, F10		;if (pnext > pfree) break
F2:
	mov32	_allocp+2,_allocp CX,SI		;_allocp = p

	mov	DS,DX
	mov	BP,[DI]
	mov	4[DI],BP		;pfree->size = *pfree

	mov32	2[DI],[DI] AX,BX	;pfree->next = pnext

	mov	DS,CX
	mov32	2[SI],[SI] DX,DI	;p->next = pfree

	mov32	AX,BX CX,SI
	add	BX,4[SI]
   ifndef PROTECTED
	normptr	AX,BX, CX		;AX,BX = p + p->size
	mov	CX,DS			;restore CX trashed by normptr
   endif
	.if32	AX,BX e DX,DI, F3	;if (p + p->size != pfree)
		mov32	CX,SI DX,DI	;p = pfree
F3:
;	while (p + p->size == p->next)
	mov	DS,CX
	mov32	AX,BX CX,SI
	add	BX,4[SI]
ifdef PROTECTED
	jc	F4
endif
	normptr	AX,BX, DX		;normalize AX,BX

	mov32	DX,DI 2[SI],[SI]	;DX:DI = p->pnext
	mov	ES,DX
	.if32	AX,BX ne DX,DI, F4

;		if (p->size + p->next->size < 64k)
		mov	BP,4[SI]
		add	BP,ES:4[DI]
    ifdef DOS16RM
	jc	F4			;Yes - then we can't combine it.
	push	BX
	dec	BP
	mov	BX, DS			;Get the segment length
	lsl	BX, BX
	cmp	BP, BX			;Have we exceeded the segment length?
	pop	BX
	jae	F5			;Yes -	overflowed a segment, all done.
	inc	BP
	mov	4[SI],BP		;No - Save the new combined size
	mov	AX,ES:[DI]		; and move next link to previous block.
	mov	[SI],AX
	mov	AX,ES:2[DI]
	mov	2[SI],AX		;Keep looping until all consecutive
	jmp	F3			; blocks have been combined.

					;We get here if the consecutive blocks
					; exceed 64K.
F5:	jne	freeerr			;If not exactly 64K, it's an error.
    else
		jc	F5
			mov	4[SI],BP	;p->size += p->next->size

			mov	AX,ES:[DI]
			mov	[SI],AX
			mov	AX,ES:2[DI]
			mov	2[SI],AX	;p->next = p->next->next
			jmp	F3
;		else
F5:
;			pnew = p + 64k - 16		;CX:SI = p
			mov	AX,CX
			add	AX,0FFFh		;AX:SI = pnew
			mov	DS,AX			;DS:SI = pnew

;			pnew->next = p->next->next	;ES:DI = p->next
			mov32	AX,BX ES:2[DI],ES:[DI]
			mov32	2[SI],[SI] AX,BX

;			pnew->size = p->size + p->next->size - (64k - 16)
			sub	BP,0FFF0h
			mov	4[SI],BP
;			p->next = pnew
			mov	AX,DS
			mov	DS,CX
			mov32	2[SI],[SI] AX,SI
;			p->size = (64k - 16)
			mov	4[SI],0FFF0h
;			p = pnew
			mov	CX,AX	;only the segments are different
			jmp	F3
    endif
F4:
	clr	AX
	cwd			;return 0L on success (for backwards compatibility)
	pop	DS
	.restore <DI,SI>
	WINLEAVE
	talloc_unlock
	ret
;
freeerr:
	pop	DS
	jmp	heaperr
c_endp	free
    endif

	endcode	alloc
	end
