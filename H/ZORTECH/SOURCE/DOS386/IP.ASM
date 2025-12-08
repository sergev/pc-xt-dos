;_ int.asm   Modified by Joe Huffman August 14, 1990
; Copyright (C) 1984-1991 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

include macros.asm

        begdata

        public  _DBL_MAX,_DBL_MIN,_FLT_MAX,_FLT_MIN

_DBL_MAX        label   byte
_DBL_MIN        label   byte
_FLT_MAX        label   byte
_FLT_MIN        label   byte

nofloat db      0Dh,0Ah,'Floating point not loaded',0Dh,0Ah
noflnend        equ     $-nofloat

        enddata

        begcode int

        extrn   __exit:near, _write:near

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

        push    noflnend
        push    offset DGROUP:nofloat
        push    2                       ;stderr
        call    _write
        add     esp,12
        call    __exit                  ;Exit without calling destructor.
        ret                             ;Just in case.



        endcode int

        end

