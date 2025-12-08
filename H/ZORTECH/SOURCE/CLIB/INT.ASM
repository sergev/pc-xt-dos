;_ int.asm   Sat Jun  3 1989   Modified by: Walter Bright
; Copyright (C) 1984-1989 by Walter Bright
; Windows support added by G. Eric Engstrom Jan 1991
; OS2 support added by Nikki Locke May 1989
; All Rights Reserved
; Written by Walter Bright

include macros.asm

_DATA	segment	word public 'DATA'

	public	_DBL_MAX,_DBL_MIN,_FLT_MAX,_FLT_MIN

_DBL_MAX	label	byte
_DBL_MIN	label	byte
_FLT_MAX	label	byte
_FLT_MIN	label	byte

    ifdef _WINDOWS
nofloat	db	'Floating point not loaded'
noflnend	equ	$-nofloat
    	db	0
    else
nofloat	db	0Dh,0Ah,'Floating point not loaded',0Dh,0Ah
noflnend	equ	$-nofloat
	db	'$'
    endif
_DATA	ends

DGROUP	group	_DATA

    ifdef __OS2__
extrn	DOSWRITE:far
extrn	DOSEXIT:far
    endif

_TEXT	segment	byte public 'CODE'
	assume CS:_TEXT,DS:DGROUP

	;From double.asm
	public	__DADD@,__DSUB@,__DMUL@,__DDIV@,__DNEG@,__DTST@
	public	__DTST0@,__DTST0EXC@
	public	__DCMP@,__DCMPEXC@
	public	__DBLINT@,__INTDBL@,__DBLUNS@,__UNSDBL@,__DBLFLT@,__FLTDBL@
	public	__DBLLNG@,__LNGDBL@,__DBLULNG@,__ULNGDBL@
	public	__DTST87@,__87TOPSW@,__DBLTO87@,__FLTTO87@,__DBLINT87@
	public	__DBLLNG87@
	public	__INTFLT@,__UNSFLT@,__LNGFLT@,__FLTINT@,__FLTUNS,__FLTLNG@
	public	___fpclassify_d

	;From float.asm
	public	__FSUB@
	public	__FADD@,__FMUL@,__FDIV@,__FNEG@,__FTST@
	public	__FTST0@,__FTST0EXC@
	public	__FCMP@,__FCMPEXC@

__DADD@:
__DSUB@:
__DMUL@:
__DDIV@:
__DNEG@:
__DTST@:
__DTST0@:
__DTST0EXC@:
__DCMP@:
__DCMPEXC@:
__DBLINT@:
__INTDBL@:
__DBLUNS@:
__UNSDBL@:
__DBLFLT@:
__FLTDBL@:
__DBLLNG@:
__LNGDBL@:
__DBLULNG@:
__ULNGDBL@:
__DTST87@:
__87TOPSW@:
__DBLTO87@:
__FLTTO87@:
__DBLINT:
__DBLINT87@:
__DBLLNG87@:
__INTFLT@:
__UNSFLT@:
__LNGFLT@:
__FLTINT@:
__FLTUNS:
__FLTLNG@:
___fpclassify_d:

__FSUB@:
__FADD@:
__FMUL@:
__FDIV@:
__FNEG@:
__FTST@:
__FTST0@:
__FTST0EXC@:
__FCMP@:
__FCMPEXC@:

	public	__FLOATCVT,__DOEXPONENT,__TRIM0
	public	_atof,_frexp,_ldexp,_strtod,_strtof
__FLOATCVT:
__DOEXPONENT:
__TRIM0:
_atof:
_frexp:
_ldexp:
_strtod:
_strtof:

    ifdef __OS2__
	sub	SP,2		;leave space for return value
	mov	BX,SP		;and remember address
	pushi	AX,2		;stderr file handle
	push	DS		;message segment
	pushi	AX,<offset DGROUP:nofloat>	;message offset
	pushi	AX,noflnend	;message length
	push	SS		;return value segment
	push	BX		;return value offset
	call	DOSWRITE
	mov	AX,1
	push	AX		;Action code 1 - terminate process
	push	AX		;exit code 1
	call	DOSEXIT
    else
    	mov	DX,offset DGROUP:nofloat
    ifndef _WINDOWS
        bdos	9
    else
        push	DS
        push	DX
        call	MESSAGEFATALERROR
    endif
	mov	AX,4C01h	;terminate a process
	bdos
    endif

_TEXT	ends

	end

