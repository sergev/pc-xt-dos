;_ cputype.asm   Sat Jan 14 1989   Modified by: Walter Bright */

include	macros.asm

	begdata
	extrn	__osmajor:byte,__osminor:byte
	enddata

	begcode	CPUTYPE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Determine which CPU is running.
; Placed into public domain by Compaq Computers.
;	int cputype(void);
; Returns:
;	0	8088/8086/V20
;	1	80186
;	2	80286
;	3	80386
;	4	80486

	c_public cputype
func	cputype
	pushf
	clr	AX
	push	AX
	popf		;try forcing flag word to all 0
	pushf
	pop	AX	;retrieve them for testing
	and	AH,0F0h
	.if	AH ne 0F0h, L2	;it's a 286 or 386

	;Distinguish between 8088/8086 and 80186
	clr	AX
	push	SP		;is SP pushed before or after it's decremented?
	pop	BX
	.if	BX ne SP,L1	;it's an 8088/8086
	jmps	L3		;it's an 80186

L2:	mov	AX,0F000h
	push	AX		;try to force high bits on in flags
	popf
	pushf
	pop	AX		;AX = what was actually stored
	and	AH,0F0h
	mov	AX,2
	jz	L1		;no, it's a 286

	;Distinguish between 386 and 486

	;Cannot use extended registers if we're using OS/2 1.0 or 1.2
	;(be careful, we could be in the compatibility box)
	.if	__osmajor ne 10, L4
	.if	__osminor be 1, L3	;assume 386
L4:
	.386
	mov	BX,SP
	and	SP,0FFFCh	;round down to a dword boundary
	pushfd
	pushfd
	pop	EDX
	mov	ECX,EDX
	xor	EDX,40000h	;toggle AC bit
	and	ECX,40000h
	push	EDX
	popfd
	pushfd
	pop	EDX
	popfd			;restore original flags
	mov	SP,BX		;restore original stack pointer
	and	EDX,40000h
	inc	AX		;AX = 3
	.if	EDX e ECX,L1	;it's a 386

L3:	inc	AX		;it's a 186 or 486
L1:	popf			;original flags
	ret
c_endp	cputype

	endcode	CPUTYPE

	end
