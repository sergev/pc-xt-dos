include macros.asm

begcode memx
c_public  memrcmp, memtest
;
;       int memrcmp(s1,s2,n)
; Returns:
;       > 0     if s1 > s2
;       = 0     if s1 == s2
;       < 0     if s1 < s2
;
;
func	memrcmp
	push    BP                                 
	mov     BP,SP                              
	.save	<SI,DI>
    if SPTR
      ife ESeqDS
	mov	AX,DS
	mov	ES,AX
      endif
	mov	DI,P+2[BP]		;get source pointer (s2)
	mov     SI,P[BP]		;get destination pointer (s1)
    else
	mov	BX,DS
	les     DI,P+SIZEPTR[BP]	;get source pointer (s2)
	lds	SI,P[BP]		;get destination pointer (s1)
    endif
	mov	CX,P+SIZEPTR+SIZEPTR[BP]		;CX = n
        add     SI,CX
        add     DI,CX
        dec     SI
        dec     DI
        std
	clr	AX
	repe	cmpsb			;compare string
	je	L3			;strings are equal
    if 0
	jae	L4			;s1 < s2
	inc	AX			;s1 > s2
	jmps	L3

L4:	dec	AX			;AX = -1
    else
	sbb	AX,AX
	cmc
	adc	AX,0
    endif
L3:
        cld
    if LPTR
	mov	DS,BX
    endif
	.restore <DI,SI>
	pop     BP                                 
	ret                                        
c_endp	memrcmp
;
;
;       int memtest(s,n)
; Returns:
;       non-zero if all s not zeros
;
;
func    memtest
        push    bp
        mov     bp,sp
        push    di
    if SPTR
        push    ds
        pop     es
        mov     di,p[BP]                ;get destination pointer (s)
    else
        les     di,p[BP]                ;get destination pointer (s)
    endif
        mov     cx,p+SIZEPTR[BP]        ;CX = n
        sub     al,al
        repe    scasb                   ;compare each byte with 0
        jnz     X1
        sub     ax,ax
        jmp     short X2
X1:     mov     ax,0ffffh
X2:     pop     di
        pop     bp                                 
        ret                                        
c_endp  memtest
;
endcode memx
        end
