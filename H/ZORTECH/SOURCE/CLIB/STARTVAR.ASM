; startvar.asm
;	defines the variables needed by the start up code
;	this allows dllstart and winstart to coexist in the same library
;	(C) Copyright 1991 Zortech, Inc.
;	All Rights Reserved
;	Written by: G. Eric Engstrom

include		macros.asm

	public	_aenvseg
	public	_dataSegSize
	public	__datapar
	public	__heapbottom
	public	__pastdata
	public	__tab_size

	begdata
__tab_size	dw	8		; default size of tabs in this system, see tabsize.h
_aenvseg	dw	0		; DLL's don't have an environment segment
_dataSegSize	dw	0
__datapar	dw	0
__heapbottom	dw	0
__pastdata	dw	0
        enddata
        end

