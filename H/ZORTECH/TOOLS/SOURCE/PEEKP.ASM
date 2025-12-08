include macros.asm

begcode peekp
;
; Peek a section of memory out of the DOS memory space into
; a buffer in the current data segment
;
; void ppeek(unsigned address, void *buf, size_t bytes);
;
extrn __x386_zero_base_selector:word

c_public ppeek
func    ppeek
        push    ebp
        mov     ebp,esp
        push    esi
        push    edi
        push    ds
        push    es
        mov     esi,P[ebp]
        mov     edi,P+4[ebp]
        mov     ecx,P+8[ebp]
        push    ds
        pop     es
        mov     ax, __x386_zero_base_selector
        mov     ds,ax
        rep     movsb
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ebp
        ret
c_endp  ppeek
;
; Poke a section of memory into the DOS memory space
; from the current data segment
;
; void ppoke(unsigned address, void *buf, size_t bytes);
;
c_public ppoke
func    ppoke
        push    ebp
        mov     ebp,esp
        push    esi
        push    edi
        push    ds
        push    es
        mov     edi,P[ebp]
        mov     esi,P+4[ebp]
        mov     ecx,P+4+SIZEPTR[ebp]
        mov     ax, __x386_zero_base_selector
        mov     es,ax
        rep     movsb
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ebp
        ret
c_endp  ppoke
endcode peekp
        end
