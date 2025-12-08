;_ alloca.asm	Tue Aug 21 1990   Modified by: Walter Bright */
;Copyright (C) 1990 by Walter Bright
;All Rights Reserved
;Written by Walter Bright

include macros.asm

	begdata
ifdef DOS386
	extrn   __heapbottom:dword, __x386_break:dword
else
	c_extrn   _pastdata,dword
endif
	enddata

	begcode alloca

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Allocate data on the stack frame.
; This is a 'magic' function that needs help from the compiler to
; work right, do not change its name, do not call it from other compilers!
; Input:
;	PS[ESP] number of bytes to allocate (nbytes)
;	-4[EBP] number of bytes in the locals in the stack frame
;		(this parameter is passed magically by the ZTC code
;		 generator, thus do not use alloca() with ZTCs prior
;		 to 2.15).
;		This is adjusted upon return to reflect the additional
;		size of the stack frame.
; Returns:
;	EAX	allocated data, NULL if nbytes is 0 or stack overflows

	c_public	alloca
func	alloca
	mov	EAX,PS[ESP]	;get nbytes
	uses	<ECX,EDX,EDI,ESI>
	add	EAX,3
	and	AL,0FCh		;round up to dword
	mov	ESI,EAX		;ESI = nbytes
	neg	EAX
	jz	Aoverflow	;alloca(0) returns NULL
	add	EAX,ESP		;EAX is now what the new ESP will be.
	jae	Aoverflow

ifdef DOS386	;is ESP off bottom?
	_ifs	EAX be <__x386_break>,Aoverflow
else
	_ifs	EAX be _pastdata,Aoverflow   ;Unlikely!! ~2 Gbytes under UNIX!
endif

	;We must copy down to [ESP] the temps on the stack.
	;The number of temps is (EBP - ESP - locals).
	mov	ECX,EBP
	sub	ECX,ESP
	sub	ECX,-4[EBP]		;ECX = number of temps (bytes) to move.
	mov	ESP,EAX			;Set up new stack pointer.
	add	EAX,ECX			;Return value = ESP + temps.
	add	-4[EBP],ESI		;adjust locals by nbytes for next
					;call to alloca().
	mov	EDI,ESP			;Destination of copy of temps.
	add	ESI,ESP			;Source of copy.
	shr	ECX,2			;ECX to count of dwords in temps
					;Always at least 4 (nbytes, EIP, ESI,
ife ESeqDS				;and EDI).
	mov	DX,DS
	mov	ES,DX
endif
	rep	movsd
	jmps	done

Aoverflow:
	;We overflowed the stack.  Return NULL.
	clr	EAX

done:
	unuse	<ESI,EDI,EDX,ECX>
	ret
c_endp	alloca

	endcode alloca

	end
