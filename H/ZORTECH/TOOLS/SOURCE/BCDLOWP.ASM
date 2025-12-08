;Low level functions for BCD arithmetic, 386 code
;Copyright Zortech Ltd 1988 - 1990
;Written by Steve Teale
;revised 11/29/1990

include macros.asm

;
;  add(n,p1,p2);
;
;  p1 and p2 point to n byte areas - result is put in p1 area
;  p2 area not affected
;  returns nonzero if overflowed
;
begcode bcdlowp
public _bcd_add
_bcd_add proc   near
        push    ebp
        mov     ebp,esp
        uses    <esi,edi>

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     esi,P+4+SIZEPTR[ebp]
        mov     edi,P+4[ebp]

        clc
bcdadd1:
        mov     al,[esi]
        adc     al,[edi]
        daa
        stosb
        inc     esi
        loop    bcdadd1
        rcl     eax,1
        and     eax,1
        unuse   <edi,esi>
        pop     ebp
        ret
_bcd_add endp
;
;  sub(n,p1,p2);
;
;  p1 and p2 point to n byte areas - result is put in p1 area
;  p2 area not affected
;  returns nonzero if overflowed
;
public  _bcd_sub
_bcd_sub proc   near
        push    ebp
        mov     ebp,esp
        uses    <esi,edi>

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     si,P+4+SIZEPTR[ebp]
        mov     di,P+4[ebp]

        clc
bcdsub1:
        mov     al,[edi]
        sbb     al,[esi]
        das
        stosb
        inc     esi
        loop    bcdsub1
        rcl     eax,1
        and     eax,1
        unuse   <edi,esi>
        pop     ebp
        ret
_bcd_sub endp
;
;  inc(n,a)    Just up it by one quickly as possible
;
public  _bcd_inc
_bcd_inc proc   near
        push    ebp
        mov     ebp,esp
        push    edi

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     edi,P+4[ebp]
bcdinc1:
        push    ecx
        mov     al,[edi]
        and     al,0fh
        cmp     al,9
        jnc     bcdinc2
        pop     ecx
        inc     byte ptr [edi]
        sub     al,al
        jmp     short bcdincex
bcdinc2:
        mov     al,[edi]
        mov     cl,4
        shr     al,cl
        cmp     al,9
        jnc     bcdinc3
        inc     al
        shl     al,cl
        pop     ecx
        stosb
        sub     al,al
        jmp     short bcdincex
bcdinc3:
        sub     al,al
        stosb
        pop     ecx
        loop    bcdinc1
        mov     eax,1                   ;signal overflow
bcdincex:
        pop     edi
        pop     ebp
        ret
_bcd_inc endp
;
;  dec(n,a)    Just knock it down by one quickly as possible
;
public  _bcd_dec
_bcd_dec proc   near
        push    ebp
        mov     ebp,esp
        push    edi

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     edi,P+4[ebp]
bcddec1:
        push    ecx
        mov     al,[edi]
        and     al,0fh
        or      al,al
        jz      bcddec2
        pop     ecx
        dec     byte ptr [edi]
        sub     al,al
        jmp     short bcddecex
bcddec2:
        mov     al,[edi]
        mov     cl,4
        shr     al,cl
        or      al,al
        jz      bcddec3
        dec     al
        shl     al,cl
        add     al,9
        pop     ecx
        stosb
        sub     al,al
        jmp     short bcddecex
bcddec3:
        mov     al,99h
        stosb
        pop     ecx
        loop    bcdinc1
        mov     eax,0ffffffffh   ;signal underflow
bcddecex:
        pop     edi
        pop     ebp
        ret
_bcd_dec endp
;
;  mul(n,p1,d);
;
;  p1 points to n byte area - result is put in p1
;  d is a digit to multiply by
;  returns nonzero if overflowed
;
public  _bcd_mul
_bcd_mul proc   near
        push    ebp
        mov     ebp,esp
        uses    <edi,ebx>

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     edi,P+4[ebp]

        mov     bx,P+4+SIZEPTR[ebp]
;        sub     bh,bh
bcdmul1:
        push    ecx              ;preserve counter
        mov     al,[edi]      ;get first digit
        and     al,0fh
        mul     bl              ;multiply
        add     al,bh           ;add any carry
        aam                     ;split into decimal digits
        mov     bh,ah
        push    eax              ;save result
        mov     al,[edi]
        mov     cl,4
        shr     al,cl            ;get second digit
        mul     bl              ;multiply
        add     al,bh           ;add any carry
        aam                     ;split into decimal digits
        mov     bh,ah
        shl     al,cl           ;move into high nibble
        pop     ecx             ;recover first digit
        add     al,cl           ;and put in low nibble
        stosb                   ;store away
        pop     ecx
        loop    bcdmul1         ;go for next
        sub     ah,ah
        mov     al,bh
        unuse   <ebx,edi>
        pop     ebp
        ret
_bcd_mul endp
;
;
;  bcd_mul10(n,p1);
;
;  p1 points to n byte area - result is put in p1
;  multiply by 10
;  returns nonzero if overflowed
;
public  _bcd_mul10
_bcd_mul10 proc near
        push    ebp
        mov     ebp,esp
        push    edi

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     edi,P+4[ebp]

        sub     bl,bl
mul10:
        push    ecx             ;preserve counter
        mov     al,[edi]        ;get first digit
        mov     dl,al
        mov     cl,4
        shr     dl,cl
        shl     al,cl
        add     al,bl
        mov     bl,dl
        stosb                   ;store away
        pop     ecx
        loop    mul10         ;go for next
        sub     ah,ah
        mov     al,bl
        pop     edi
        pop     ebp
        ret
_bcd_mul10 endp
;
;  div10(n,p1);
;
;  p1 points to n byte area - result is put in p1
;  divide by 10
;  no return value
;
public  _bcd_div10
_bcd_div10 proc near
        push    ebp
        mov     ebp,esp
        push    edi

ife ESeqDS
        mov     ax,ds
        mov     es,ax
endif

        mov     ecx,P[ebp]
        mov     edi,P+4[ebp]

        add     edi,ecx
        dec     edi             ;point at most significant
        sub     bl,bl
        std                     ;direction backwards
div10:
        push    ecx             ;preserve counter
        mov     al,[edi]        ;get most significant digits
        mov     dl,al
        mov     cl,4
        shl     dl,cl
        shr     al,cl
        add     al,bl
        mov     bl,dl
        stosb                   ;store away
        pop     ecx
        loop    div10           ;go for next
        cld
        pop     edi
        pop     ebp
        ret
_bcd_div10 endp
;
endcode bcdlowp
        end
