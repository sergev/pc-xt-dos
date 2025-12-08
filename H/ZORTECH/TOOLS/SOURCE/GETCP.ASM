include macros.asm

begcode getcp
public  __getcp
__getcp proc  near
        push    ebx
        mov     ah,3
        xor     bh,bh
        int     10h
        mov     eax,edx
        pop     ebx
        ret
__getcp endp
endcode getcp
        end
