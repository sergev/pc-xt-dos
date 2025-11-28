
; ----------------------------------------------------------
; SOURCE CODE FOR TURBO.COM
;
; COMPILE USING NASM VERSION 2.16.01, OR LATER.
; ----------------------------------------------------------

BITS 16

CPU 8086

ORG 0100H

; Display the banner text. 
	mov	ah,9
	mov	dx,BANNER
	int	21h

; If no command line argument, display program usage text, then exit.
	cmp	byte [0080h],0
	je	DO_USAGE

; For the 'cmpsb' instruction below, just in case, ensure that the direction flag is set to increment.
	cld

; If command line argument is '/off' or '/OFF', then turn turbo off.
; Note that at address 81h in the PSP will be a space.
	mov	si,0082h
	mov	di,CLA_OFF_1	; '/off'
	mov	cx,4		; 4 bytes to compare
	repe	cmpsb
	je	OFF
	mov	si,0082h
	mov	di,CLA_OFF_2	; '/OFF'
	mov	cx,4		; 4 bytes to compare
	repe	cmpsb
	je	OFF

; Command line argument is neither '/off' nor '/OFF'.
; If command line argument is '/on' or '/ON', then turn turbo on.
; Note that at address 81h in the PSP will be a space.
	mov	si,0082h
	mov	di,CLA_ON_1	; '/on'
	mov	cx,3		; 3 bytes to compare
	repe	cmpsb
	je	ON
	mov	si,0082h
	mov	di,CLA_ON_2	; '/ON'
	mov	cx,3		; 3 bytes to compare
	repe	cmpsb
	je	ON

; Invalid or no command line argument. Display program usage.
DO_USAGE:
	mov	ah,9
	mov	dx,TEXT_USAGE
	int	21h
	jmp	EXIT

; Turn turbo OFF.
OFF:	mov	ah,9
	mov	dx,TEXT_OFF
	int	21h
	in	al,61h
	and	al,0FBh		; Clear bit 2
	out	61h,al
	jmp	EXIT

; Turn turbo ON.
ON:	mov	ah,9
	mov	dx,TEXT_ON
	int	21h
	mov	al,61h
	or	al,04h		; Set bit 2
	out	61h,al

; DOS program exit 
EXIT:	mov	ax,4C00h
	int	21h

BANNER:
	db	'For XT-class motherboards that control turbo functionality via',13,10
	db	'bit 2 of port B on the 8255 chip.',13,10,13,10,'$'

TEXT_OFF:
	db	'Turning turbo: OFF',13,10,'$'

TEXT_ON:
	db	'Turning turbo: ON',13,10,'$'

TEXT_USAGE:
	db	'Usage:',13,10
	db	'   TURBO /OFF will turn turbo off',13,10
	db	'   TURBO /ON  will turn turbo on',13,10
	db	'$'

CLA_OFF_1:	db	'/off'
CLA_OFF_2:	db	'/OFF'
CLA_ON_1:	db	'/on'
CLA_ON_2:	db	'/ON'

