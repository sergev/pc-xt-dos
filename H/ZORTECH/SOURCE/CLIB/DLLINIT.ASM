;_ dllinit.asm   Thu May 17 1990   Modified by: Walter Bright */
; DLL initialization code for OS/2
; Calls static constructors

include	macros.asm

	DOSSEG		;have linker fix ordering of segments

;Define this to prevent linker from pulling in C.ASM
	public	__acrtused
__acrtused equ	1234

	begdata
	public	__module_handle
__module_handle	dw	?	;Module handle of DLL
	enddata

	extrn	__cinit_f:far

_TEXT	segment word public 'CODE'
	assume	CS:_TEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; On input:
;	SS:SP	Current user stack
;	DS	Selector for DLL's DGROUP, if any, otherwise, selector
;		for DGROUP of process attaching to DLL.
;		C and C++ DLLs always have a DGROUP.
; For OS/2:
;	AX	module handle of the DLL
; Returns:
;	AX	0 error, !=0 success


LibEntry proc	far
	mov	__module_handle,AX
	call	__cinit_f		; call static constructors
        ret
LibEntry endp

_TEXT	ends

	end	LibEntry
