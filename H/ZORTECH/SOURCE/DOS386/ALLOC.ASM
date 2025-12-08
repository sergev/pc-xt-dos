;_ alloc.asm
; Modified by Joe Huffman June 21, 1990
; Copyright (C) 1985-1990 by Walter Bright
; All rights reserved
; Written by Walter Bright

include macros.asm

	extrn _sbrk:near, _write:near, __exit:near

	public _malloc,_calloc,_realloc,_free

; Storage allocator

	begdata

	public __baslnk
	c_extrn _pastdata,dword, _heapbottom,dword

__baslnk	dd	offset DGROUP:__baslnk	;starting link for
						; storage allocator
		dd	0	;give it a size of 0 so it is never allocated
__allocp	dd	offset DGROUP:__baslnk	;roving pointer for allocator

_allocp equ	__allocp
_baslnk equ	__baslnk

heapmsg		db	0Ah,'Heap is corrupted',0Ah
heapmsglen	equ	$-heapmsg

; A block in the free list consists of:
free_block	struc
next	dd	?	;pointer to next block in list
f_size	dd	?	;size of block in bytes including free_block struc
free_block	ends	;(must be multiple of 4)


; When it's allocated,
used_block	struc
u_size	dd	?	;# of bytes in this block including this struc
used_block	ends

	public	__malloc_handler

__malloc_handler	dd	offset _TEXT: default_malloc_handler

	enddata

	begcode alloc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; When out of memory, this function pointer is called in an attempt to get more.
; The function pointer can be set by the application to point at a function
; that can, say, free up some disk buffers.
; Returns:
;	0	unsuccessful at finding more memory to free
;	!=0	found some, try allocating it

default_malloc_handler proc    near
	clr	eax		;didn't find any
	ret
default_malloc_handler endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data and clear it.
; Use:
;	p = calloc(numelems,sizeof(elem));
; Returns:
;	pointer to allocated data else NULL

	_align

_calloc	proc	near
	mov	eax,PS[esp]	;get numelems
	mov	edx,PS+4[esp]	;get sizeof(elem)
	_ifs	edx e 1, C1	;no need to multiply
	mul	edx

	jc	short C3	;if overflow
C1:	push	eax		;nbytes

	callm	malloc
	add	esp,4

	tst	eax		;fail?
	jz	short C2	;yes

	uses	<ecx,edx,edi>
	mov	edi,eax
	mov	edx,eax		;save pointer to result
	mov	ecx,-4[edi]	;# of bytes
	shr	ecx,2		;# of dwords (including byte count)
	dec	ecx		;skip byte count

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax
	rep	stosd		;clear the memory
	mov	eax,edx		;restore pointer to result
	unuse	<edi,edx,ecx>
C2:
	ret

C3:	clr	eax
	ret

_calloc	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate a block of data.
; Use:
;	char *malloc();
;	p = malloc(nbytes);
; Returns:
;	pointer to allocated data else NULL

	_align

_malloc	proc	near
	push	ebp
A4:
	mov	ebp,esp
	uses	<ebx,ecx,edx,esi,edi>

	mov	eax,P[ebp]	;get nbytes
	add	eax,7		;need another dword for length info

	and	AL,NOT 3	;round up to nearest dword
	_ifs	eax b 7, allocerr	;can't allocate 0 bytes

	mov	ebp,4		;save some bytes
	mov	esi,_baslnk	;last item
	mov	ecx,esi		;CX to save bytes
	jmps	A2

	_align
A1:	mov	esi,edi
	_ifs	esi e ecx, trysbrk	;wrapped around, didn't find any
A2:	mov	edi,[esi]		;next item in list
	_ifs	eax a [edi+ebp], A1	;not big enough

	je	short A3	;exactly big enough
	add	eax,ebp		;we'll need another 4 bytes
	_ifs	eax e [edi+ebp],A3 ;have to allocate an entire block
	sub	eax,ebp

;Allocate from bottom of free block.  Desirable in order to delay
;stack overflow as long as possible.
; edi -> free block
; esi -> previous free block
; eax =  # of bytes in allocated block
	add	[esi],eax	;link to new free block
	mov	esi,[esi]	;pointer to new free block
	mov	ecx,[edi+ebp]	;number of bytes in block we're splitting
	sub	ecx,eax		;CX = remaining bytes
	mov	[esi+ebp],ecx	;# of bytes in this block

A3:	xchg	eax,[edi]	;[EDI] = # of bytes, EAX = next free block
	mov	[esi],eax	;skip the DI entry in list
	mov	_allocp,esi
	lea	eax,[edi+ebp]	;pointer to area allocated (EDI + 4)

A6:	unuse	<edi,esi,edx,ecx,ebx>
	pop	ebp
	ret

trysbrk:			;try sbrk() to grow our data segment
	_ifs	eax ae 04000h, A5
	mov	eax,04000h	;16K byte minimum increment.
A5:	push	eax
	callm	sbrk
	pop	ebx
	_ifs	eax e -1, allocerr	 ;failed

	mov	[eax],ebx	;Number of bytes allocated.
	add	eax,4		;point past # of bytes allocated
	push	eax
	callm	free		;add allocated memory into free list
	add	esp,4
	unuse	<edi,esi,edx,ecx,ebx>
	jmp	A4		;try again

allocerr:
	clr	EAX		;NULL
	jmp	A6
_malloc	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Reallocate memory that was allocated by malloc() or calloc().
; Use:
;	char *realloc(char *p, unsigned nbytes)
; Returns:
;	0 error
;	else pointer to reallocated memory

R6:	pop	ebp
	jmp	_free			;free(p)

R5:	;function just like malloc(nbytes)
	push	eax
	callm	malloc
	add	esp,4

	unuse	<edx>
	pop	ebp
	ret

_realloc proc	near
	push	ebp
	mov	ebp,esp

	mov	eax,P+SIZEPTR[ebp]	;EAX = nbytes
	tst	eax			;trying to realloc() to 0 size?
	jz	R6			;yes

	;if realloced size is smaller, attempt to just shrink current block
	uses	<edx>

	mov	edx,P[ebp]	;EDX = p
	tst	edx		;is p NULL?
	jz	R5		;yes

	uses	<ecx,esi,edi>
	sub	edx,4
	mov	ecx,[edx]	;ECX = # of bytes in this block
	add	eax,7
	and	al,NOT 3	;EAX = real new size

	sub	ecx,eax

	jb	short R3		;if allocating more bytes
	_ifs	ecx b <SIZEPTR+4>, R4	;size of free list entry

	mov	[edx],eax	;realloced size of p
	add	edx,eax
	mov	[edx],ecx	;size of new fragment
	add	edx,4
	push	edx
	callm	free
	add	esp,4
R4:
	mov	eax,P[ebp]	;reload original pointer p
	jmps	R1		;no change, return p

	;we'll have to allocate a new block, and free the old one
R3:
	push	P+SIZEPTR[ebp]
	callm	malloc		;malloc(nbytes)
	add	esp,4
	tst	eax

	jz	rallocerr	;error
	push	eax		;save pointer to new memory

	mov	esi,P[ebp]	;DS:ESI -> original
	mov	edi,eax		;ES:EDI -> new item
	mov	ecx,-4[esi]
	_ifs	ecx be -4[edi], R2
	mov	ecx,-4[edi]	;ECX = smaller of two size
R2:	shr	ecx,2		;# of dwords
	dec	ecx		;compensate for extra dword in beginning

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	rep	movsd		;transfer the dwords
	push	P[ebp]
	callm	free		;free the old one
	add	esp,SIZEPTR

	tst	eax
	pop	eax		;restore pointer to new memory
	jnz	rallocerr
R1:	unuse	<edi,esi,ecx,edx>
	pop	ebp
	ret

rallocerr:
	clr	eax
	jmp	R1
_realloc endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Free memory that was allocated by malloc() or calloc().
; Use:
;	free(p);

_free	proc	near
	push	ebp
	mov	ebp,esp
	uses	<ebx,ecx,esi,edi>
	mov	ebx,P[ebp]		;get p
	tst	ebx			;pass a NULL pointer?
	jz	F5			;yes, return 0
	mov	ebp,SIZEPTR		;to save some bytes
					;check if below bottom of pool
	_ifs	ebx be _heapbottom, heaperr	;if below bottom of pool
	_ifs	ebx ae _pastdata, heaperr	;if above top of pool
	test	bl,3			;Not multiple of 4?
	jne	heaperr
	sub	ebx,ebp			;point to start of block
	mov	eax,[ebx]		;# of bytes in block to be freed

; Try to find ESI and EDI such that ESI < EBX < EDI

	mov	esi,_allocp		;try our roving pointer
	_ifs	esi b ebx, F1		;a good starting point
	mov	esi, offset DGROUP:_baslnk
	mov	edi,esi

	_align
F6:	mov	esi,edi
F1:	mov	edi,[esi]		;the next in the list
	_ifs	esi ae ebx, heaperr
	_ifs	edi a ebx, F2		;got it
	_ifs	edi a esi, F6		;no wrap around (ESI < EDI < EBX)

; We have ESI < EBX < EDI (relative position in list)

F2:	mov	ecx,[esi+ebp]		;# of bytes in previous block
	add	ecx,esi			;+ link
	_ifs	ecx ne ebx, F3		;if can't collapse with prev block
	add	[esi+ebp],eax		;Number of bytes in block to be freed.
	jmps	F4

F3:	mov	[ebx+ebp],eax		;store # of bytes in freed block
	mov	[ebx],edi		;link to block after EBX
	mov	[esi],ebx		;link to EBX
	mov	esi,ebx

; See if we can collapse SI with DI
; SI -> block just before DI
; DI -> block just after SI
; BP = SIZEPTR

F4:	mov	_allocp,esi		;for next time
	mov	eax,[esi+ebp]
	add	eax,esi
	_ifs	eax ne edi, F5		;nope
	mov	eax,[edi]		;link after EDI
	mov	[esi],eax		;becomes link after ESI
	mov	eax,[edi+ebp]		;# of bytes in EDI
	add	[esi+ebp],eax		;add to # of bytes in ESI

F5:	clr	eax			;success
F7:	unuse	<edi,esi,ecx,ebx>
	pop	ebp
	ret

_free	endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; The heap has been corrupted. Print an error message and abort the program.

heaperr proc	near
	push	heapmsglen
	push	offset DGROUP:heapmsg
	push	2		;Stderr
	callm	write

	push	1		;error exit code
	call	__exit
	hlt			;Should never reach here.
heaperr endp

	endcode alloc
	end
