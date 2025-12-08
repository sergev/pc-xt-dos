;_ cputype.asm   Sat Jan 14 1989   Modified by: Walter Bright */

include	macros.asm

	begcode	CPUTYPE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Determine which CPU is running.
;	int cputype(void);
; Returns:
;	3	80386
;	4	80486

	c_public cputype
func	cputype
	mov	EAX,3		;assume 80386

	;This code that distinguishes a 386 from a 486 depends on
	;the 386's inability to toggle the AC bit in the EFLAGS register,
	;but the 486 can. This technique is apparently blessed by Intel.

	uses	<EBX,ECX,EDX>
	mov	EBX,ESP
	and	ESP,0FFFFFFFCh	;align down to nearest dword
	pushfd			;save original flags
	pushfd
	pop	EDX
	mov	ECX,EDX
	xor	EDX,40000h	;toggle AC bit
	push	EDX
	popfd
	pushfd
	pop	EDX
	and	EDX,40000h
	and	ECX,40000h
	_ifs	EDX e ECX, L1	;it's a 386
	inc	EAX		;it's a 486
L1:	popfd			;restore original flags
	mov	ESP,EBX		;restore original ESP
	unuse	<EDX,ECX,EBX>

	ret
c_endp	cputype

	endcode	CPUTYPE

	end
