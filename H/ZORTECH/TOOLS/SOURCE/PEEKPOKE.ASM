include macros.asm

begcode peekpoke
;
; Peek a section of memory out of the DOS memory space into
; a buffer in the current data segment
;
; void ppeek(unsigned address, void *buf, size_t bytes);
;
c_public ppeek
func    ppeek
        push    ebp
        mov     ebp,esp
        uses    <esi,edi,eax,ecx,ds,es>
        mov     esi,P[ebp]
        mov     edi,P+4[ebp]
        mov     ecx,P+4+SIZEPTR[ebp]
        mov     ax,ds
        mov     es,ax
        mov     ax,0060h
        mov     ds,ax
        rep     movsb
        unuse   <es,ds,ecx,eax,edi,esi>
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
        uses    <esi,edi,eax,ecx,ds,es>
        mov     edi,P[ebp]
        mov     esi,P+4[ebp]
        mov     ecx,P+4+SIZEPTR[ebp]
        mov     ax,0060h
        mov     es,ax
        rep     movsb
        unuse   <es,ds,ecx,eax,edi,esi>
        pop     ebp
        ret
c_endp  ppoke
endcode peekpoke
        end
