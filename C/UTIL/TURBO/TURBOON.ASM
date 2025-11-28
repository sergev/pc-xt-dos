
; ----------------------------------------------------------
; SOURCE CODE FOR TURBOON.COM
;
; COMPILE USING NASM VERSION 2.16.01, OR LATER.
; ----------------------------------------------------------

BITS 16

CPU 8086

ORG 0100H

; Display the text.
mov	ah,9
mov	dx,TEXT
int	21h

; Turn on turbo.
in	al,61h
or	al,04h		; Set bit 2
out	61h,al

EXIT:	mov	ax,4C00h
	int	21h

TEXT:
db	'For XT-class motherboards that control turbo functionality via',13,10
db	'bit 2 of port B on the 8255 chip.',13,10
db	13,10
db	'Turning turbo: ON'
db	13,10,'$'