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
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
if LPTR
        mov     cx,p+SIZEPTR+SIZEPTR[bp]  ;get length in words
        lds     si,p+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+SIZEPTR+SIZEPTR[bp]  ;get length in words
        mov     si,p+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        clc
lintadd1:
        mov     ax,ds:[si]
        adc     ax,es:[di]
        stosw
        inc     si
        inc     si
        loop    lintadd1
        rcl     ax,1
        and     ax,1
        pop     di
        pop     si
        pop     ds
        pop     bp
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
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
if LPTR
        mov     cx,p+SIZEPTR+SIZEPTR[bp]  ;get length in words
        lds     si,p+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+SIZEPTR+SIZEPTR[bp]  ;get length in words
        mov     si,p+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        clc
lintsub1:
        mov     ax,es:[di]
        sbb     ax,ds:[si]
        stosw
        inc     si
        inc     si
        loop    lintsub1
        rcl     ax,1
        and     ax,1
        pop     di
        pop     si
        pop     ds
        pop     bp
        ret
c_endp  lint_sub
;
;  lint_negate(p1,n);
;
;  p1 points to n word area - result is put in p1
;
func    lint_negate
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+SIZEPTR[bp]  ;get length in words
        les     di,p[bp]
else
        mov     cx,p+SIZEPTR[bp]  ;get length in words
        mov     di,p[bp]
        push    ds
        pop     es
endif
        stc
lintneg1:
        mov     ax,es:[di]
        not     ax
        adc     ax,0
        stosw
        loop    lintneg1
        pop     di
        pop     bp
        ret
c_endp  lint_negate
;
;  lint_inc(p,n)    p points at n byte area to be incremented
;
func    lint_inc
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        stc
lintinc1:
        mov     ax,es:[di]
        adc     ax,0
        stosw
        loop    lintinc1
        rcl     ax,1
        and     ax,1
        pop     di
        pop     bp
        ret
c_endp  lint_inc
;
;  lint_dec(p,n)    p points at n byte area to be incremented
;
func    lint_dec
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        stc
lintdec1:
        mov     ax,es:[di]
        sbb     ax,0
        stosw
        loop    lintdec1
        rcl     ax,1
        and     ax,1
        pop     di
        pop     bp
        ret
c_endp  lint_dec
;
;  lint_mul(p1,d,n);
;
;  p1 points to n byte area - result is put in p1
;  d is a digit to multiply by
;  returns nonzero if overflowed
;
func    lint_mul
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+2+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+2+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        xor     bx,bx
lintmul1:
        mov     ax,es:[di]      ;get first digit
        mov     dx,p+SIZEPTR[bp]
        mul     dx              ;multiply by d
        add     ax,bx           ;add any carry
        adc     dx,0
        mov     bx,dx           ;save carry
        stosw                   ;store result away
        loop    lintmul1        ;go for next
        mov     ax,bx           ;overflow
        pop     di
        pop     bp
        ret
c_endp  lint_mul
;
;  lint_div(p1,d,n);
;
;  p1 points to nbyte area - result is put in p1
;  d is a digit to divide by
;  check for division by zero before calling
;
func    lint_div
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+2+SIZEPTR[bp]
        mov     bx,p+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+2+SIZEPTR[bp]
        mov     bx,p+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        add     di,cx
        add     di,cx
        dec     di
        dec     di
        xor     dx,dx
        std
lintdiv1:
        mov     ax,es:[di]      ;get first digit
        mov     bx,p+SIZEPTR[bp]
        div     bx
        stosw
                                ;remainder is in dx for next digit
        loop    lintdiv1        ;go for next
        cld
        mov     ax,dx           ;remainder
        pop     di
        pop     bp
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
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+2+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+2+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        xor     bx,bx
lintls1:
        push    cx
        xor     dx,dx
        mov     ax,es:[di]      ;get next digit
        mov     cx,p+SIZEPTR[bp]      ;get shift count
lintls2:
        sal     ax,1
        rcl     dx,1            ;carry into dx
        loop    lintls2
        or      ax,bx
        mov     bx,dx
        stosw                   ;store result away
        pop     cx
        loop    lintls1         ;go for next
        mov     ax,dx           ;overflow
        pop     di
        pop     bp
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
        push    bp
        mov     bp,sp
        push    di
if LPTR
        mov     cx,p+2+SIZEPTR[bp]
        les     di,p[bp]
else
        mov     cx,p+2+SIZEPTR[bp]
        mov     di,p[bp]
        push    ds
        pop     es
endif
        add     di,cx
        add     di,cx
        push    cx

        dec     di
        dec     di              ;point at most significant word
        std                     ;set direction flag
        mov     ax,es:[di]
        mov     bx,ax
        and     bx,08000h
        mov     cx,p+SIZEPTR[bp]
        sar     bx,cl           ;get suitable top bits in bx
        pop     cx
lintrs1:
        push    cx
        xor     dx,dx
        mov     ax,es:[di]      ;get next digit
        mov     cx,p+SIZEPTR[bp]      ;get shift count
lintrs2:
        shr     ax,1            ;lowest n-1 bytes shifted logically
        rcr     dx,1            ;carry into dx
        loop    lintrs2
        or      ax,bx           ;adjust vacated bits
        mov     bx,dx           ;save shifted bits for next digit
        stosw                   ;store result away
        pop     cx
        loop    lintrs1         ;go for next
        mov     ax,dx           ;underflow
        cld                     ;reset direction flag
        pop     di
        pop     bp
        ret
c_endp  lint_rshift
        endcode lintn
        end
