;_ page.asm   Wed May 24 1989   Modified by: Walter Bright */
; Copyright (C) 1989-1990 by Walter Bright
; All rights reserved
; Written by Walter Bright

include	macros.asm

; Storage allocator
; The page layout is set up to be binary compatible with the
; 16 bit version.

	begcode	page

	c_public page_malloc,page_calloc,page_realloc,page_free

; The start of each page (set by page_initialize()) looks like:
pageheader struc
	pagesize	dw	?	;total size of this page
	maxsize		dw	?	;max size of a free block in free list
	allocp		dw	?	;roving pointer for allocator
	bassize		dw	?	;size of first block (0 so it
					; is never allocated)
	baslnk		dw	?	;offset of next free block
pageheader ends
pageoverhead	equ	10	;# of bytes of bookkeeping

; A block in the free list consists of:
;	dw	size of block in bytes (must be even) (including both words)
;	dw	pointer to next block in list

; When it's allocated,
;	dw	# of bytes in this block including this word
;	db...	the bytes allocated


;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data and clear it.
; Use:
;	unsigned page_calloc(void *baseptr,unsigned size);
; Returns:
;	offset of allocated data else 0

func	page_calloc
	push	EBP
	mov	EBP,ESP
	push	P+4[EBP]
	push	P[EBP]
	callm	page_malloc	;allocate a chunk
	mov	ESP,EBP
	tst	EAX		;out of memory?
	jz	short C2	;yes
	uses	<EDI>
	mov	EDI,P[EBP]	;baseptr
	add	EDI,EAX		;offset to start of allocated memory
	mov	EDX,EAX		;save offset of result
	movzx	ECX,word ptr -2[EDI]	;# of bytes
	shr	ECX,1		;# of words (including byte count)
	dec	ECX		;skip byte count
	clr	EAX
	push	DS
	pop	ES
	rep	stosw		;clear the memory
	mov	EAX,EDX		;restore offset of result
	unuse	<EDI>
C2:
	pop	EBP
	ret
c_endp	page_calloc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data.
;	unsigned page_malloc(void *baseptr,unsigned size);
; Returns:
;	offset of allocated data else 0

func	page_malloc
	push	EBP
	mov	EBP,ESP
	uses	<EBX,ESI,EDI>
	mov	EBX,P[EBP]	;DS:EBX = baseptr
	mov	EAX,P+4[EBP]	;get size
	add	EAX,3		;need another word for length info
	and	AL,0FEh		;round up to nearest word
	_ifs	EAX ae 10000h-2, allocerr	;can't allocate > 64k
	_ifs	EAX b 4, allocerr		;can't allocate 0 bytes

;	movzx	ESI,allocp[EBX]	;last item
	movzx	ESI,baslnk[EBX]	;last item
	mov	ECX,ESI		;ECX to save bytes
	jmps	A2

A1:	mov	ESI,EDI
	_ifs	ESI e ECX, allocerr		;wrapped around, didn't find any
A2:	movzx	EDI,word ptr 2[EBX][ESI]	;next item in list
	_ifs	AX a [EBX+EDI], A1	;not big enough

	je	A3		;exactly big enough
	add	EAX,2		;we'll need another 2 bytes
	_ifs	AX e [EBX+EDI],A3	;have to allocate an entire block
	sub	EAX,2

;Allocate from bottom of free block.
; EDI -> free block
; ESI -> previous free block
; EAX =  # of bytes in allocated block
	add	2[EBX+ESI],AX	;link to new free block
	mov	SI,2[EBX+ESI]	;pointer to new free block
	mov	CX,[EBX+EDI]	;number of bytes in block we're splitting
	sub	ECX,EAX		;ECX = remaining bytes
	mov	[EBX+ESI],CX	;# of bytes in this block
	mov	[EBX+EDI],AX	;[EDI] = # of bytes

A3:	mov	AX,2[EBX+EDI]	;EAX = next free block
	mov	2[EBX+ESI],AX	;skip the EDI entry in list
	mov	allocp[EBX],SI
	lea	EAX,2[EDI]	;pointer to area allocated (EDI + 2)
A6:	mov	maxsize[EBX],0	;recalculate size of largest available block
	unuse	<EDI,ESI,EBX>
	pop	EBP
	ret

allocerr:
	clr	EAX		;NULL
	jmp	A6
c_endp	page_malloc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Reallocate memory that was allocated by page_malloc() or page_calloc().
; Use:
;	unsigned page_realloc(void *baseptr,unsigned p, unsigned nbytes)
; Returns:
;	0 error
;	else offset of reallocated memory

func	page_realloc
	push	EBP
	mov	EBP,ESP

	mov	EAX,P+4+4[EBP]		;EAX = nbytes
	tst	EAX			;trying to realloc() to 0 size?
	jnz	R6			;no
	pop	EBP
	jmp	near ptr page_free	;page_free(baseptr,p)

R6:	;If p is 0, this is just a page_malloc()
	mov	EDX,P+4[EBP]		;EDX = p
	tst	EDX			;is p NULL?
	jnz	R5			;no
	;function just like page_malloc(baseptr,nbytes)
	push	EAX
	push	P[EBP]
	callm	page_malloc
	mov	ESP,EBP
	pop	EBP
	ret

	;if realloced size is smaller, attempt to just shrink current block
R5:	push	ESI
	mov	ESI,P[EBP]		;ESI = baseptr
	sub	EDX,2
	movzx	ECX,word ptr [ESI+EDX]	;ECX = # of bytes in this block
	add	EAX,3
	and	AL,0FEh			;EAX = real new size
	sub	ECX,EAX
	jb	R3			;if allocating more bytes
	_ifs	ECX b 4, R4		;size of free list entry
	mov	[ESI+EDX],AX		;realloced size of p
	add	EDX,EAX			;EDX -> new fragment
	mov	[ESI+EDX],CX		;size of new fragment
	add	EDX,2
	push	EDX
	push	P[EBP]
	callm	page_free
	add	ESP,8
R4:
	mov	EAX,P+4[EBP]
	jmps	R1			;no change, return p

	;we'll have to allocate a new block, copy the data over,
	;and free the old one
R3:
	push	P+8[EBP]
	push	P[EBP]
	callm	page_malloc	;page_malloc(baseptr,nbytes)
	add	ESP,8
	tst	EAX
	jz	rallocerr	;error
	push	EAX		;save pointer to new memory
	uses	<EDI>
	push	DS
	pop	ES
	mov	EDI,EAX
	add	EDI,ESI		;ES:EDI -> new item
	add	ESI,P+4[EBP]	;DS:ESI -> original
	movzx	ECX,word ptr -2[ESI]
	_ifs	CX be -2[EDI], R2
	mov	CX,-2[EDI]	;ECX = smaller of two sizes
R2:	shr	ECX,1		;# of words
	dec	ECX		;compensate for extra word in beginning
	rep	movsw		;transfer the words
	push	P+4[EBP]
	push	P[EBP]
	callm	page_free	;free the old one
	add	ESP,8
	unuse	<EDI>
	tst	EAX
	pop	EAX		;restore pointer to new memory
	jz	R1
rallocerr:
	clr	EAX
R1:	pop	ESI
	pop	EBP
	ret
c_endp	page_realloc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Free memory that was allocated by page_malloc() or page_calloc().
; Use:
;	int page_free(void *baseptr,unsigned p);
; Returns:
;	0	success
;	-1	error

func	page_free
	push	EBP
	mov	EBP,ESP
	mov	EAX,P+4[EBP]		;get p
	tst	EAX			;pass a NULL pointer?
	jz	F8			;yes, return 0
	mov	EDX,EAX
	uses	<EBX,ESI,EDI>
	mov	EBX,P[EBP]		;DS:EBX = baseptr
	_if	EDX be baslnk+1, freeerr	;if below bottom of pool
	_if	EDX ae 10000h, freeerr		;if high 16 bits is not clear
	_ifs	DX ae pagesize[EBX], freeerr	;if above top of pool
	test	DL,1			;odd?
	jne	freeerr
	sub	EDX,2			;point to start of block
	mov	ESI,EDX
	movzx	EAX,word ptr [EBX+ESI]	;# of bytes in block to be freed

; Try to find ESI and EDI such that ESI < EDX < EDI

	mov	SI,allocp[EBX]		;try our roving pointer
	_ifs	ESI b EDX, F1		;a good starting point
	mov	ESI, bassize
	jmps	F1

F6:	mov	ESI,EDI
F1:	movzx	EDI,word ptr 2[EBX+ESI]	;the next in the list
	_ifs	ESI ae EDX, freeerr
	_ifs	EDI a EDX, F2		;got it
	_ifs	EDI a ESI, F6		;no wrap around (ESI < EDI < EDX)

; We have ESI < EDX < EDI (relative position in list)

F2:	movzx	ECX,word ptr [EBX+ESI]	;# of bytes in previous block
	add	ECX,ESI			;+ link
	_ifs	ECX ne EDX, F3		;if can't collapse with prev block
	add	[EBX+ESI],AX
	jmps	F4

F3:	mov	2[EBX+ESI],DX		;link to EDX
	mov	ESI,EDX
	mov	2[EBX+ESI],DI		;link to block after EDX

; See if we can collapse ESI with EDI
; ESI -> block just before EDI
; EDI -> block just after ESI

F4:	mov	allocp[EBX],SI		;for next time
	mov	AX,[EBX+ESI]
	add	EAX,ESI
	_ifs	EAX ne EDI, F5		;nope
	mov	AX,2[EDI+EBX]		;link after EDI
	mov	2[ESI+EBX],AX		;becomes link after ESI
	mov	AX,[EBX+EDI]		;# of bytes in EDI
	add	[EBX+ESI],AX		;add to # of bytes in ESI

F5:	clr	EAX			;success
F7:	mov	maxsize[EBX],0		;recalc max size
	unuse	<EDI,ESI,EBX>
F8:	pop	EBP
	ret

freeerr:
	mov	EAX,-1			;error
	jmp	F7
c_endp	page_free


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Determine size of largest free block in page.
;	unsigned page_maxfree(void far *baseptr);

	c_public page_maxfree
func	page_maxfree
	push	EBP
	mov	EBP,ESP
	push	ESI
	mov	ESI,P[EBP]	;DS:ESI = baseptr
	movzx	EAX,maxsize[ESI]
	tst	EAX
	jnz	M3		;does not need recalculation
	clr	EAX
	movzx	EDX,baslnk[ESI]		;offset to first free block
M1:	movzx	ECX,word ptr [ESI+EDX]	;size of free block
	_ifs	ECX b EAX, M2
	mov	EAX,ECX
M2:	mov	CX,2[ESI+EDX]	;offset to next link
	xchg	ECX,EDX
	_ifs	EDX a ECX, M1
	sub	EAX,2
	jc	M4
	mov	maxsize[ESI],AX
M3:	pop	ESI
	pop	EBP
	ret

M4:	clr	EAX
	jmp	M3
c_endp	page_maxfree


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Initialize memory allocation system in a page.
; Page size must be < 64k.
;	unsigned page_initialize(void *baseptr,unsigned pagesize);
; Returns:
;	size of largest allocatable block

	c_public page_initialize
func	page_initialize
	push	EBP
	mov	EBP,ESP
	mov	EDX,P[EBP]		;DS:EDX = baseptr
	movzx	EAX,word ptr P+4[EBP]	;EAX = pagesize
	mov	pagesize[EDX],AX
	sub	EAX,pageoverhead
	mov	allocp[EDX],bassize
	mov	bassize[EDX],0
	mov	baslnk[EDX],pageoverhead

	;Construct one big free block of the remainder
	mov	pageoverhead[EDX],AX	;size of that block
	mov	word ptr pageoverhead+2[EDX],bassize

	sub	EAX,2			;overhead per allocated block
	mov	maxsize[EDX],AX

	pop	EBP
	ret
c_endp	page_initialize

	endcode	page
	end
