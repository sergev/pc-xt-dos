sgn             equ      80000000h      ;mask for sign bit
sgn_mask        equ      7fffffffh      ;Mask off sign bit
shortexp        equ      7F800000h      ;mask for short exponent
shorthid        equ      00800000h      ;mask for hidden bit
shortbias       equ            7Fh      ;exponent bias
short_qnan      equ      7FC00000h      ;short quit NaN
fqnan_bit       equ        400000h      ;Quite NaN bit for float
short_infinity  equ      7F800000h

longexp         equ      7FF00000h      ;mask for long exponent
longhid         equ      00100000h      ;mask for hidden bit
mantisa_mask    equ      000fffffh      ;mask out Mantisa
longbias        equ           3FFh      ;exponent bias
long_qnan       equ      7FF80000h      ;quiet NaN
dqnan_bit       equ         80000h      ;Quite NaN bit for double
long_infinity   equ      7FF00000h



dtype_other	equ	00h
dtype_zero	equ	01h
dtype_infinite	equ	02h
dtype_snan	equ	03h		;signaling NaN index
dtype_qnan	equ	04h		;quite NaN index
dtype_base	equ	5

FP_NANS         equ     0
FP_NANQ         equ     1
FP_INFINITE     equ     2
FP_NORMAL       equ     3
FP_SUBNORMAL    equ     4
FP_ZERO         equ     5



; Exceptions
FE_INVALID	equ	1
FE_DENORMAL	equ	2
FE_DIVBYZERO	equ	4
FE_OVERFLOW	equ	8
FE_UNDERFLOW	equ	10h
FE_INEXACT	equ	20h
FE_ALL_EXCEPT	equ	3Fh

; Rounding directions
FE_TONEAREST	equ	0
FE_DOWNWARD	equ	400h
FE_UPWARD	equ	800h
FE_TOWARDZERO	equ	0C00h

; Rounding precision
FE_FLTPREC	equ	0
FE_DBLPREC	equ	200h
FE_LDBLPREC	equ	300h

sh32StyRBy8       macro
        local   notSticky32
        or	AL,AL
        je      notSticky32
        or	AH,1

notSticky32:
        shr     EAX,8
	endm

xChgReg64 macro
        xchg    EDX,ECX
        xchg    EAX,EBX
	endm

neg64   macro
	neg	EDX
	neg	EAX
	sbb	EDX,0
	endm

shl64	macro	r1,r2
	shl	r2,1
	rcl	r1,1
	endm

shr64	macro	r1,r2
	shr	r1,1
	rcr	r2,1
	endm

sh64LBy8  macro
	shld	EDX,EAX,8
	shl	EAX,8
	endm

sh64RBy8  macro
        shrd    EAX,EDX,8
	shr	EDX,8
	endm


sh64StyRBy8       macro
        or      AH,AL
        shrd    EAX,EDX,8
	shr	EDX,8
        endm

; Floating point environment
fenv_t	struc
	;Mimic 8087 status
	;load with default values
	status	dw	0
	control	dw	1000h or FE_TONEAREST or FE_LDBLPREC or FE_ALL_EXCEPT
	round	dw	FE_TONEAREST
	res1	dw	0		;reserved
	res2	dw	0		;reserved
fenv_t	ends

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set floating point exception bits

feexcept macro	bits
    ifdef _MT
	push	bits
	call	__FEEXCEPT
    else
	or	__fe_cur_env.status,bits
    endif
	endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get rounding mode in EAX

feround	macro
    ifdef _MT
	call	__FEROUND
    else
	movzx	EAX,__fe_cur_env.round
    endif
	endm


