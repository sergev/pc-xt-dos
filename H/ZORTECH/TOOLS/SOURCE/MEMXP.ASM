include macros.asm

;
;       int memrcmp(s1,s2,n)
; Returns:
;       > 0     if s1 > s2
;       = 0     if s1 == s2
;       < 0     if s1 < s2
;
;
begcode memxp
public  _memrcmp
_memrcmp proc   near
        push    ebp
        mov     ebp,esp
        uses    <esi,edi>

      ife ESeqDS
        mov     ax,ds
        mov     es,ax
      endif

        mov     edi,P+SIZEPTR[ebp]  ;get source pointer (s2)
        mov     esi,P[ebp]          ;get destination pointer (s1)
        mov     ecx,P+SIZEPTR+SIZEPTR[ebp]  ;CX = n
        add     esi,ecx
        add     edi,ecx
        dec     esi
        dec     edi

        std                 ;remember!

        clr     eax
        repe    cmpsb       ;compare string
        je      short L3    ;strings are equal
        sbb     eax,eax
        cmc
        adc     eax,0
L3:
        cld
        unuse <edi,esi>
        pop     ebp                         
        ret                                        
_memrcmp endp
;
;
;       int memtest(s,n)
; Returns:
;       non-zero if all s not zeros
;
;
public  _memtest
_memtest proc   near
        push    ebp
        mov     ebp,esp
        push    edi

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     edi,P[ebp]              ;get destination pointer (s)

        mov     ecx,P+SIZEPTR[ebp]      ;CX = n
        sub     al,al
        repe    scasb                   ;compare each byte with 0
        jnz     X1
        sub     eax,eax
        jmp     short X2
X1:     mov     eax,0ffffffffh
X2:     pop     edi
        pop     ebp                                 
        ret                                        
_memtest endp
endcode memxp
;
        end
