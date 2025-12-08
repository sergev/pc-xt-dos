include macros.asm
WORDS   equ     4
        begcode lintn
c_public  lint_add, lint_sub, lint_inc, lint_dec, lint_negate
c_public  lint_mul, lint_div, lint_lshift, lint_rshift
;
;  lint_add(p1,p2,n);
;
;  p1 and p2 point to n word areas - result is put in p1 area
;  p2 area not affected
;  returns nonzero if overflowed
;
func    lint_add
        push    ebp
        mov     ebp,esp
        uses    <esi,edi>

        mov     ecx,P+SIZEPTR+SIZEPTR[ebp]  ;get length in words
        mov     esi,P+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        clc
lintadd1:
        mov     ax,[esi]
        adc     ax,[edi]
        stosw
        inc     esi
        inc     esi
        loop    lintadd1
        rcl     eax,1
        and     eax,1
        unuse   <edi,esi>
        pop     ebp
        ret
c_endp  lint_add
;
;  lint_sub(p1,p2,n);
;
;  p1 and p2 point to n byte areas - result is put in p1 area
;  p2 area not affected
;  returns nonzero if overflowed
;
func    lint_sub
        push    ebp
        mov     ebp,esp
        uses    <esi,edi>

        mov     ecx,P+SIZEPTR+SIZEPTR[ebp]  ;get length in words
        mov     esi,P+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        clc
lintsub1:
        mov     ax,word ptr [edi]
        sbb     ax,word ptr [esi]
        stosw
        inc     esi
        inc     esi
        loop    lintsub1
        rcl     eax,1
        and     eax,1
        unuse   <edi,esi>
        pop     ebp
        ret
c_endp  lint_sub
;
;  lint_negate(p1,n);
;
;  p1 points to n word area - result is put in p1
;
func    lint_negate
        push    ebp
        mov     ebp,esp
        push    edi

        mov     ecx,P+SIZEPTR[ebp]  ;get length in words
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        stc
lintneg1:
        mov     ax,[edi]
        not     ax
        adc     ax,0
        stosw
        loop    lintneg1
        pop     edi
        pop     ebp
        ret
c_endp  lint_negate
;
;  lint_inc(p,n)    p points at n byte area to be incremented
;
func    lint_inc
        push    ebp
        mov     ebp,esp
        push    edi

        mov     ecx,P+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        stc
lintinc1:
        mov     ax,[edi]
        adc     ax,0
        stosw
        loop    lintinc1
        rcl     eax,1
        and     eax,1
        pop     edi
        pop     ebp
        ret
c_endp  lint_inc
;
;  lint_dec(p,n)    p points at n byte area to be incremented
;
func    lint_dec
        push    ebp
        mov     ebp,esp
        push    edi

        mov     ecx,P+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        stc
lintdec1:
        mov     ax,[edi]
        sbb     ax,0
        stosw
        loop    lintdec1
        rcl     eax,1
        and     eax,1
        pop     edi
        pop     ebp
        ret
c_endp  lint_dec
;
;  lint_mul(p1,d,n);
;
;  p1 points to n (size_t) byte area - result is put in p1
;  d is a 16 bit digit to multiply by
;  returns nonzero if overflowed
;
func    lint_mul
        push    ebp
        mov     ebp,esp
        uses    <edi,ebx>

        mov     ecx,P+4+SIZEPTR[ebp]    ;stack is four bytes wide
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        xor     bx,bx
lintmul1:
        mov     ax,[edi]        ;get first digit
        mov     edx,P+SIZEPTR[ebp]
                                ;don't care about high bytes
        mul     dx              ;multiply by d
        add     ax,bx           ;add any carry
        adc     dx,0
        mov     bx,dx           ;save carry
        stosw                   ;store result away
        loop    lintmul1        ;go for next
        movzx   eax,bx          ;overflow
        unuse   <ebx,edi>
        pop     ebp
        ret
c_endp  lint_mul
;
;  lint_div(p1,d,n);
;
;  p1 points to n byte area - result is put in p1
;  d is a digit to divide by
;  check for division by zero before calling
;
func    lint_div
        push    ebp
        mov     ebp,esp
        uses    <edi,ebx>

        mov     ecx,P+4+SIZEPTR[ebp]
        mov     ebx,P+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        add     edi,ecx
        add     edi,ecx
        dec     edi
        dec     edi
        xor     dx,dx
        xor     eax,eax
        std
lintdiv1:
        mov     ax,[edi]        ;get first digit
;        mov     ebx,P+SIZEPTR[ebp]
        div     bx
        stosw
                                ;remainder is in dx for next digit
        loop    lintdiv1        ;go for next
        cld
        movzx   eax,dx          ;remainder
        unuse   <ebx,edi>
        pop     ebp
        ret
c_endp  lint_div
;
;
;  lint_lshift(p1,d,n);
;
;  p1 points to n byte area - result is put in p1
;  d is shift count
;
func    lint_lshift
        push    ebp
        mov     ebp,esp
        uses    <edi,ebx>

        mov     ecx,P+4+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        xor     bx,bx       ;clears carry
lintls1:
        push    ecx
        xor     dx,dx
        mov     ax,[edi]    ;get next digit
        mov     ecx,P+SIZEPTR[ebp]      ;get shift count
lintls2:
        sal     ax,1
        rcl     dx,1            ;carry into dx
        loop    lintls2
        or      ax,bx           ;combine
        mov     bx,dx           ;store carries for next combine
        stosw                   ;store result away
        pop     ecx
        loop    lintls1         ;go for next
        movzx   eax,dx          ;overflow
        unuse   <ebx,edi>
        pop     ebp
        ret
c_endp  lint_lshift
;
;
;  lint_rshift(p1,d,n);
;
;  p1 points to n byte area - result is put in p1
;  d is shift count
;  does an arithmetic shift, negative compound integers stay negative
;
func    lint_rshift
        push    ebp
        mov     ebp,esp
        uses    <edi,ebx>

        mov     ecx,P+4+SIZEPTR[ebp]
        mov     edi,P[ebp]
        mov     ax,ds
        mov     es,ax

        add     edi,ecx
        add     edi,ecx
        push    ecx

        dec     edi
        dec     edi             ;point at most significant word
        std                     ;set direction flag
        mov     ax,[edi]
        mov     bx,ax
        and     bx,08000h
        mov     ecx,P+SIZEPTR[ebp]
        sar     bx,cl           ;get suitable top bits in bx
        pop     ecx
lintrs1:
        push    ecx
        xor     dx,dx
        mov     ax,[edi]      ;get next digit
        mov     ecx,P+SIZEPTR[ebp]      ;get shift count
lintrs2:
        shr     ax,1            ;lowest n-1 bytes shifted logically
        rcr     dx,1            ;carry into dx
        loop    lintrs2
        or      ax,bx           ;adjust vacated bits
        mov     bx,dx           ;save shifted bits for next digit
        stosw                   ;store result away
        pop     ecx
        loop    lintrs1         ;go for next
        movzx   eax,dx          ;underflow
        cld                     ;reset direction flag
        unuse   <ebx,edi>
        pop     ebp
        ret
c_endp  lint_rshift
        endcode lintn
        end
