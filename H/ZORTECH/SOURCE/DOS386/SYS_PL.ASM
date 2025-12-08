;;;;;;;;;;;;;;;;;;;;;;;;;
; C compiler system calls for Phar Lap DOS386.
;
; Copyright Walter Bright
; Modified by Joe Huffman September 17, 1990
; Voice: 208-263-1695
; FAX:	 208-263-8772
;;;;;;;;;;;;;;;;;;;;;;;;;

include macros.asm

	ifdef	Aseg

	begcode seg
;;;;;;;;;;;;;;;;;;;;;;;;;
; Return data segment.

public	_getDS

_getDS	proc	near
	clr	EAX
	mov	AX,DS
	ret
_getDS	endp

;;;;;;;;;;;;;;;;;;;;;;;;;
; Make a far pointer from its components.
; Use: MK_FP(unsigned short seg, unsigned offset);
; Returns: DX:EAX
; August 18, 1990
;;;;;;;;;;;;;;;;;;;;;;;;;
public	      _MK_FP

_MK_FP	proc	near
	movzx	edx,word ptr PS[ESP]
	mov	eax,PS + 4 [ESP]
	ret
_MK_FP	endp

;;;;;;;;;;;;;;;;;;;;;;;;;
; Get the segment of a far pointer.
; Use: FG_SEG (fp)
; Returns: segment in AX.
; August 18, 1990
;;;;;;;;;;;;;;;;;;;;;;;;;
public	_FP_SEG
_FP_SEG proc	near
	movzx	eax,word ptr PS + 4[ESP]
	ret
_FP_SEG endp

	endcode seg

	endif	;Aseg

	ifdef A_exit
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Exit the program.
; October 4, 1990
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	begcode _exit
c_public _exit
func	_exit
	mov	AL,PS[ESP]
	bdos	4ch
	hlt			;Just in case it fails.
c_endp	_exit
	endcode	_exit

	endif	;_exit

	ifdef Amemlock
begdata
        extrn  _errno:dword
enddata
begcode
;;;;;;;;;;;;;;;;;;;;;;;;;
; Lock a region of memory in a virtual memory environment
; Use: _x386_memlock(far pointer, length)
; far pointer to start of region to lock
; Returns: 0 if success, -1 if failure
; May 26,1991
;;;;;;;;;;;;;;;;;;;;;;;;;
public __x386_memlock
__x386_memlock  proc    near
        mov     AX,501h         ;lock page, far pointer
mem_man_enter:                  ;entry point for other mem management functions
        push    EBP
        mov     EBP,ESP
        uses    <EBX,ECX,EDX,ES>
        mov     EBX,EAX                         ;proper value of BX was in AX
        mov     AX,252bh
        mov     EDX,P[SIZEPTR+SIZEPTR+EBP]      ;length in bytes
        les     ECX,P[EBP]                      ;ES:ECX points to start
        int     21h
        jnc     mem_man_no_err
        mov     _errno,9
mem_man_no_err:
        sbb     EAX,EAX
        unuse   <ES,EDX,ECX,EBX>
        pop     EBP
        ret
__x386_memlock  endp

;;;;;;;;;;;;;;;;;;;;;;;;;
; Unlock a region of memory for paging in a virtual memory environment
; Use: _x386_memunlock(far pointer, length)
; far pointer to start of region to unlock
; Returns: 0 if success, -1 if failure
; May 26,1991
;;;;;;;;;;;;;;;;;;;;;;;;;
public __x386_memunlock
__x386_memunlock        proc    near
        mov     AX,601h
        jmps    mem_man_enter
__x386_memunlock        endp

endcode
	endif	;memlock
	end

