;_ strings.asm	 Modified by Joe Huffman November 6, 1990
;Copyright (C) 1985-1989 by Walter Bright
;All Rights Reserved
;Written by Walter Bright

;CAUTION!  This code is highly optimized.  It does not fully utilize the
;macros 'uses' and 'unuse' for register preservation.  If registers other than
;EBX, ESI, and EDI (and of course EBP,DS, and ES) must be preserved examine 
;this module carefully.

include macros.asm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Compare a string.
; Use:
;	int strcmp(s1,s2)
; Returns:
;	> 0	if s1 > s2
;	= 0	if s1 == s2
;	< 0	if s1 < s2
;

	ifdef	Astrcmp
	begcode strcmp
public	_strcmp
_strcmp proc	near
	push	esi			;save ESI
	mov	edx,edi			;save EDI

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	edi,P+SIZEPTR[esp]	;get source pointer (s2)
	mov	esi,P[esp]		;get destination pointer (s1)

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb
	not	ecx			;CX = string length of s2
	sub	edi,ecx			;point DI back to beginning
	repe	cmpsb			;compare string
	je	short L1		;strings are equal

	sbb	eax,eax
	cmc
	adc	eax,0

L1:
	mov	edi,edx
	pop	esi
	ret

_strcmp endp
	endcode strcmp
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Compare a string of at most n chars (unsigned).
; Use:
;	int strncmp(s1,s2,n)
; Returns:
;	> 0	if s1 > s2
;	= 0	if s1 == s2
;	< 0	if s1 < s2
;

	ifdef	Astrncmp
	begcode strncmp
	public	_strncmp
_strncmp	proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,edx,esi,edi>

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	edi,P+SIZEPTR[ebp]	;get source pointer (s2)
	mov	esi,P[ebp]		;get destination pointer (s1)
	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb
	not	ecx			;ECX = string length of s2
	sub	edi,ecx			;point EDI back to beginning

	mov	edx,P+(2*SIZEPTR)[ebp]
	_if	ecx b edx,L5
	mov	ecx,edx			;ECX = min(ECX,n)

L5:	tst	eax			;Set ZF.
	repe	cmpsb			;compare string
	je	short L3		;strings are equal
	sbb	eax,eax
	cmc
	adc	eax,0
L3:
	unuse	<edi,esi,edx,ecx>
	pop	ebp
	ret
_strncmp	endp
	endcode strncmp
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Compare an array of n bytes (unsigned).
; Use:
;	int memcmp(s1,s2,n)
; Returns:
;	> 0	if s1 > s2
;	= 0	if s1 == s2
;	< 0	if s1 < s2
;

	ifdef	Amemcmp
	begcode memcmp
	public	_memcmp
_memcmp proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,esi,edi>

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	edi,P+SIZEPTR[ebp]	;get source pointer (s2)
	mov	esi,P[ebp]		;get destination pointer (s1)
	mov	ecx,P+(2*SIZEPTR)[ebp]	;ECX = n
	clr	eax
	cld
	repe	cmpsb			;compare string
	je	short L3		;strings are equal
	sbb	eax,eax
	cmc
	adc	eax,0
L3:
	unuse	<edi,esi,ecx>
	pop	ebp
	ret
_memcmp endp
	endcode memcmp
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Calculate length of string and return it.
;	int strlen(s)

	ifdef	Astrlen
	begcode strlen
	public	 _strlen

_strlen proc	near
	mov	edx,edi			;save EDI
	mov	edi,PS[esp]		;EDI = s

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb
	mov	eax,ecx
	not	eax			;EAX = string length
	dec	eax
	mov	edi,edx			;Restore EDI
	ret
_strlen endp
	endcode strlen
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Cat s2 to s1 till a zero byte.
; Use:
;	char *strcat(s1,s2)
; Returns:
;	s1
;
	ifdef	Astrcat
	begcode strcat
	public	_strcat
_strcat proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,ebx,edx,esi,edi>

	mov	edi,P+SIZEPTR[ebp]	;s2

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb			;find end of s1
	not	ecx			;ECX = strlen(s2) + 1 (for EOS)
	sub	edi,ecx			;back up EDI to start of s2

	mov	edx,ecx			;EDX = strlen(s2)
	mov	ecx,eax
	dec	ecx			;ECX = -1
	mov	esi,edi
	mov	edi,P[ebp]		;EDI = s1
	mov	ebx,edi			;EBX = s1
	repne	scasb
	dec	edi			;EDI -> EOS of s1
	mov	ecx,edx			;ECX = strlen(s2)
	shr	ecx,2
	rep	movsd
	mov	ecx,3
	and	ecx,edx
	rep	movsb			;transfer bytes (including EOS)
	mov	eax,ebx			;return pointer to s1

	unuse	<edi,esi,edx,ebx,ecx>
	pop	ebp
	ret
_strcat endp
	endcode strcat
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Cat s2 to s1 till a zero byte or n bytes are copied.
; Use:
;	char *strncat(char *s1,char *s2,unsigned n)
; Returns:
;	s1
;
	ifdef	Astrncat
	begcode strncat
	public	_strncat
_strncat	proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,edx,esi,edi>

	mov	esi,P+SIZEPTR[ebp]	;get source pointer (s2)
	mov	edi,P[ebp]		;get destination pointer (s1)

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb			;find end of s1
	dec	edi			;point at terminating 0
	mov	edx,edi			;save end of s1
	mov	edi,esi			;EDI -> s2
	mov	ecx,eax
	dec	ecx			;ECX = -1
	repne	scasb
	not	ecx
	dec	ecx			;ECX = strlen(s2)

	mov	edi,edx			;EDI -> end of s1

	mov	edx,P+SIZEPTR+SIZEPTR[ebp]
	_if	ecx b edx, L6
	mov	ecx,edx			;ECX = min(ECX,n)
L6:	mov	edx,ecx
	shr	ecx,2
	rep	movsd
	mov	cl,3
	and	ecx,edx
	rep	movsb			;transfer bytes
	stosb				;terminate with a 0
	mov	eax,P[ebp]		;return pointer to s1

	unuse	<edi,esi,ecx,edx>
	pop	ebp
	ret

_strncat	endp
	endcode strncat
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy s2 to s1 till a zero byte.
; Use:
;	char *strcpy(s1,s2)
;
	ifdef	Astrcpy
	begcode strcpy
	public	_strcpy
_strcpy proc	near
	push	esi
	mov	edx,edi
	mov	edi,P+SIZEPTR[esp]	;get source pointer (s2)
	mov	esi,edi			;save it

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb			;find end of s2
	not	ecx			;ECX = strlen(s2) + 1 (for EOS)
	mov	eax,P[esp]		;return value (s1)
	mov	edi,eax			;EDI -> s1
	rep	movsb			;transfer bytes (including EOS)
	mov	edi,edx
	pop	esi
	ret
_strcpy endp
	endcode strcpy
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy s2 to s1 till a zero byte.
; Use:
;	char *stpcpy(char *s1,const char *s2)
; Returns:
;	pointer to terminating NUL
; Written by Bob Stout and Walter Bright
; Modified by Joe Huffman June 25, 1990

	ifdef	Astpcpy
	begcode stpcpy
	public	_stpcpy
_stpcpy proc	near
	push	esi
	mov	edx,edi
	mov	edi,P+SIZEPTR[esp]	;get source pointer (s2)
	mov	esi,edi			;save it

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb			;find end of s2
	not	ecx			;ECX = strlen(s2) + 1 (for EOS)
	mov	edi,P[esp]		;EDI -> s1
	mov	eax,ecx			;return value
	add	eax,edi
	mov	edx,ecx
	shr	ecx,2
	rep	movsd
	mov	cl,3
	and	ecx,edx
	rep	movsb			;transfer bytes (including EOS)
	mov	eax,edi
	dec	eax			;EAX = s1 + strlen(s2)
	mov	edi,edx
	pop	esi
	ret
_stpcpy endp
	endcode stpcpy
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy exactly n chars from s2 to s1, padding with nulls if necessary.
; Use:
;	char *strncpy(s1,s2,n)
;
	ifdef	Astrncpy
	begcode strncpy
	public	_strncpy
_strncpy	proc	near
	push	ebp
	mov	ebp,esp
	uses	<ebx,ecx,edx,esi,edi>

	mov	edi,P+SIZEPTR[ebp]	;get source pointer
	mov	ebx,edi			;save it

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	clr	eax			;scan for 0
	mov	ecx,eax
	dec	ecx			;largest possible string (ECX = -1)
	cld
	repne	scasb			;find end of s2
	not	ecx			;ECX = strlen(s2) + 1 (for EOS)
	mov	edx,P+(2*SIZEPTR)[ebp]	;EDX = n
	_if	edx a ecx, L7		;if n > strlen(s2) + 1
	mov	ecx,edx			;ECX = min(ECX,n)
L7:
	mov	eax,ecx
	sub	edx,ecx			;EDX = # of nulls to pad
	mov	esi,ebx			;ESI -> s2
	mov	edi,P[ebp]		;EDI -> s1
	mov	ebx,edi			;Save s1 for return value.
	shr	ecx,2
	rep	movsd
	mov	cl,3
	and	ecx,eax
	rep	movsb			;transfer bytes (including EOS)
	mov	ecx,edx			;# of nulls to pad
	clr	al
	shr	ecx,2
	rep	stosd
	mov	cl,3
	and	ecx,edx
	rep	stosb
	mov	eax,ebx			;return value
	unuse	<edi,esi,edx,ecx,ebx>
	pop	ebp
	ret
_strncpy	endp
	endcode strncpy
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set n bytes in s to c.
;	char *memset(char *p,int c,int n)
; Returns:
;	p

	ifdef	Amemset
	begcode memset
	public	_memset
_memset proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,edx,edi>

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	edi,P[ebp]			;p
	mov	ecx,P+SIZEPTR+4[ebp]		;n
	movzx	eax,byte ptr P+SIZEPTR[ebp]	;c
	cld

;;;;;;;;;;;;;;;;;;;;;;;;;
;Calculation to determine method cutoff point.
;Bytes	   EAX		  AL
; n	30 + 5 * n/4	19 + 5 * n
;
;	30 + 5 * n/4 <= 19 + 5 * n
;	11 + 5 * n/4 <= 5 * n
;    5 * n - 5 * n/4 >= 11
;    5 * n * (.75)   >= 11
;	n	     >= 2.93
;But since in order for EAX method to work it requires 4 or bytes, cutoff 
;should be for n >= 4.	This is a very small memory size and unlikely to
;occur.	 We save another 3 clocks (and 7 bytes) if we don't do the cmp and
;jump at all!  The cost is:
;	      30 + 0/5/10/15 for n = 0/1/2/3
;instead of:
;	      14 + 0/5/10/15 for n = 0/1/2/3
;
;	 cmp	 ecx,4
;	 jl	 short_mem		 ;(9/3)

	mov	ah,al			;(2) Set up a 32 bit pattern.
	mov	edx,eax			;(2)
	shl	edx,16			;(3)
	or	eax,edx			;(2) EAX has the 32 bit pattern.

	mov	edx,ecx			;(2) Save the count of bytes.
	shr	ecx,2			;(2) Number of dwords.
	rep	stosd			;(5 + 5n)
	mov	cl,3			;(2)
	and	ecx,edx			;(2) Fill in the remaining odd bytes.

short_mem:
	rep	stosb			;(5 + 5n)
	mov	eax,P[ebp]
	unuse	<edi,edx,ecx>
	pop	ebp
	ret
_memset endp
	endcode memset
	endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copy n bytes from p2 to p1.
;	void *memcpy(void *p1,void *p2,n)
; Returns:
;	p1

    ifdef Amemcpy
	public	_memcpy, _memmove
	begcode memcpy

_memmove:	;Alternate entry point.
_memcpy proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,edx,edi,esi>

	mov	edi,P[ebp]		;p1
	mov	esi,P+SIZEPTR[ebp]	;p2
	mov	edx,P+(2*SIZEPTR)[ebp]	;EDX = n

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	eax,3			;Handy constant to have around.
	cmp	esi,edi
	jz	short memcpy_done	;Do nothing if same.
	ja	short memcpy_forward	;if forward copy

	add	esi,edx			;Adjust for a reverse copy.
	dec	esi
	add	edi,edx
	dec	edi
	mov	ecx,eax			;Calculate mod4(n) (EAX = 3).
	and	ecx,edx
	std				;reverse direction of copy
	rep	movsb			;Move 0->3 bytes.

	mov	ecx,edx			;EDX has n.
	shr	ecx,2			;Compute number of dwords.

	sub	esi,eax			;Adjust ESI and EDI by 3.
	sub	edi,eax
	rep	movsd
	cld
	jmps	memcpy_done

memcpy_forward:
	mov	ecx,edx			;EDX = n.
	shr	ecx,2			;Number of dwords
	cld
	rep	movsd			;Transfer the dwords.
	mov	ecx,eax			;Compute remainder (EAX = 3).
	and	ecx,edx			;EDX has n.
	rep	movsb			;Transfer the bytes.

memcpy_done:
	mov	eax,P[ebp]		;Return value.
	unuse	<esi,edi,edx,ecx>
	pop	ebp
	ret

_memcpy endp
	endcode memcpy
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	void movedata(unsigned short srcseg,unsigned srcoff,
;		unsigned short destseg,unsigned destoff,size_t numbytes);

    ifdef Amovedata
	begcode		movedata

	public	_movedata
_movedata	proc	near
	push	ebp
	mov	ebp,esp
	push	ds
	push	es
	uses	<esi,edi>

	mov	ds,P[ebp]
	mov	esi,P+4[ebp]
	mov	es,P+8[ebp]
	mov	eax,P+16[ebp]	;get numbytes
	mov	edi,P+12[ebp]
	mov	ecx,eax
	shr	ecx,2		;convert to dword count
	repz	movsd

	mov	cl,3
	and	ecx,eax		;Compute remainder.
	rep	movsb		;move remainder bytes

	unuse	<edi,esi>
	pop	es
	pop	ds
	pop	ebp
	ret
_movedata	endp
	endcode movedata
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Return pointer to first occurrence of char c in string s.
;	char *strchr(const char *s, int c);

    ifdef	Aindex
	begcode strchr
    if 0
	c_public index
func	index
c_endp	index
    endif

	c_public strchr
func	strchr
    ife ESeqDS
        mov	ax,ds
        mov	es,ax
    endif
	mov	edx,edi			;Save edi
	mov	edi,PS[esp]		;EDI = s
	clr	eax
	mov	ecx,eax
	dec	ecx			;ECX = -1
	repne	scasb
	not	ecx			;ECX = length of s (including NULL)
	sub	edi,ecx			;EDI = s
	mov	al,PS+SIZEPTR[esp]	;AL = c
	repne	scasb			;scan for c
	mov	eax,ecx			;assume we didn't find it
	jnz	short L8		;didn't find it
	mov	eax,edi			;yes, found it
	dec	eax
L8:	mov	edi,edx			;Restore edi
	ret
c_endp	strchr
	endcode strchr
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Search for char c in first n bytes pointed to by p.
;	void *memchr(const void *p,int c,size_t n)
; Returns:
;	pointer to char found, else NULL.

    ifdef	Amemchr
	begcode memchr
	public	_memchr

_memchr proc	near
	push	ebp
	mov	ebp,esp
	mov	ecx,P+SIZEPTR+4[ebp]	;ECX = n
	jecxz	short L9		;return NULL
	uses	<edi>

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	mov	edi,P[ebp]		;EDI = s
	mov	al,P+SIZEPTR[ebp]	;AL = c
	repne	scasb			;scan for c
	mov	eax,ecx			;assume we didn't find it
	jnz	short L8		;didn't find it (return NULL)
	mov	eax,edi			;yes, found it
	dec	eax
L8:	unuse	<edi>
	pop	ebp
	ret

L9:	mov	eax,ecx
	pop	ebp
	ret
_memchr endp
	endcode memchr
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Compare strings just like strcmp(), except that case is ignored.

    ifdef Astrcmpl
	begcode strcmpl

	public	_stricmp
_stricmp:

	public	_strcmpl
_strcmpl	proc	near
	push	ebp
	mov	ebp,esp
	mov	edx,esi
	uses	<ecx,edi>
	mov	esi,P[ebp]
	mov	edi,P+SIZEPTR[ebp]

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	_align
L1:	lodsb				;AL = *p1++
	mov	cl,[edi]
	inc	edi			;CL = *p2++
	_if	AL ne CL, L2		;strings are different at this char
	tst	AL			;end of string?
	jnz	short L1		;no
	jmps	L3

L2:	;Perhaps when converted to lower case, they will be the same
	_if	al b 'A', L3
	_if	al a 'Z', L4		;Already lower case?
	add	al,'a' - 'A'		;convert AL to lower case
L4:	_if	cl b 'A', L3
	_if	cl a 'Z', L5		;Already lower case?
	add	cl,'a' - 'A'		;convert CL to lower case
L5:	_if	al e cl, L1		;same now, so continue looping

L3:	clr	ah
	mov	ch,ah
	movzx	eax,ax
	movzx	ecx,cx
	sub	eax,ecx

	unuse	<edi,ecx>
	mov	esi,edx
	pop	ebp
	ret
_strcmpl	endp
	endcode strcmpl
    endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Compare strings just like memcmp(), except that case is ignored.

    ifdef Amemicmp
	begcode memicmp
	public	_memicmp
_memicmp	proc	near
	push	ebp
	mov	ebp,esp
	uses	<ecx,edx,esi,edi>
	mov	esi,P[ebp]
	mov	edi,P+SIZEPTR[ebp]
	mov	ecx,P+SIZEPTR+SIZEPTR[ebp] ;ECX = numbytes
	jecxz	short match

	mov	ah,'A'
	mov	dh,'Z'

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif

	_align
L1:	lodsb				;AL = *p1++
	mov	dl,[edi]
	inc	edi			;DL = *p2++
	_if	al ne dl, L2		;strings are different at this char
	loop	L1			;try next char
	jmps	match

L2:	;Perhaps when converted to lower case, they will be the same
	_if	al b ah, nomatch
	_if	al a dh, L4
	add	al,'a' - 'A'		;convert AL to lower case
L4:	_if	dl b ah, nomatch
	_if	dl a dh, L5
	add	dl,'a' - 'A'		;convert DL to lower case
L5:	_if	al ne dl, nomatch	;different, we're done
	loop	L1
match:	mov	eax,ecx			;return 0 in eax
	unuse	<edi,esi,edx,ecx>
	pop	ebp
	ret

nomatch:
	movzx	edx,dl
	movzx	eax,al
	sub	eax,edx
	unuse	<edi,esi,edx,ecx>
	pop	ebp
	ret
_memicmp	endp
	endcode memicmp
    endif


    ifdef A_memint
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int *_memintset(int *, int value, size_t num)
; Set an array of ints to a value.  Return the pointer to the array.
;

	begcode _memint
	public	__memintset
__memintset	proc	near

ife ESeqDS
	mov	ax,ds
	mov	es,ax
endif
	mov	edx,edi			;Save edi
	mov	edi,PS[esp]

	mov	eax,PS + SIZEPTR[esp]	 ;value
	mov	ecx,PS + SIZEPTR + 4[esp];num
	rep	stosd

	mov	edi,edx			;Restore edi.
	mov	eax,PS[esp]		;Return value.
	ret

__memintset	endp
	endcode _memint
    endif ;A_memint

	end

