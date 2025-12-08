;_ time2.asm   Sun Jun  4 1989   Modified by: Walter Bright */
; Copyright (C) 1987-1990 by Walter Bright
; All Rights Reserved
; Written by Walter Bright

include	macros.asm

;Offsets into struct tm (must match time.h values)
tm_sec	=	0
tm_min	=	4
tm_hour	=	8
tm_mday	=	12
tm_mon	=	16
tm_year	=	20
tm_wday	=	24
tm_yday	=	28
tm_isdst =	32

ifdef Atime2

	c_public	time
	public		__mdays

	begdata

;/**************************
; * # of days in year at start of month
; */

__mdays	dw	0,31,31+28,31+28+31,31+28+31+30
	dw	31+28+31+30+31,31+28+31+30+31+30,31+28+31+30+31+30+31
	dw	31+28+31+30+31+30+31+31,31+28+31+30+31+30+31+31+30
	dw	31+28+31+30+31+30+31+31+30+31,31+28+31+30+31+30+31+31+30+31+30
	dw	365

	enddata

SECSPERHOUR =	(60*60)
SECSPERDAY  =	(SECSPERHOUR*24)

	begcode	time2

;/****************************************
; * Return the number of seconds that have elapsed since the start
; * of 1980.
; *	time_t time(time_t *timer);
; * Input:
; *	timer	pointer to where to store result (or NULL)
; * Output:
; *	*timer = result (unless timer == NULL)
; * Returns:
; *	time
; */

func	time
	push	EBP
	mov	EBP,ESP
	uses	<EBX>

	; compute EAX = # of seconds since midnight
	bdos	2Ch		;get time
	movzx	EAX,CH		;hours (0..23)
	imul	EAX,60		;AX = hours * 60
	movzx	ECX,CL		;ECX = minutes
	add	EAX,ECX		;AX = hours * 60 + minutes	
	mov	CL,DH		;ECX = seconds
	imul	EAX,60		;DX,AX = ((hours * 60) + minutes) * 60
	add	EAX,ECX
	push	EAX		;save for later

	;Get the date and convert it to seconds since 1980
	bdos	2Ah		;get date
	movzx	ECX,CX
	sub	ECX,1980	;ECX = years since 1980

	movzx	EAX,DH
	dec	EAX		;month (0..11)

	movzx	EDX,DL		;EDX = day (1..31)

;  if (month <= 1 || year & 3)	/* if before Feb or not a leap year	*/
	_ifs	AL be 1, L4E
	test	CL,3
	je	L51
L4E:	dec	EDX		; don't add day for leap year
L51:

	movzx	EBX,__mdays[EAX*2]
	add	EBX,EDX		;day += __mdays[month];	(day in year)

;  day += (year + 3) >> 2;	/* add a day for each leap year		*/
	mov	EAX,ECX
	add	EAX,3
	shr	EAX,2
	add	EBX,EAX

;  t = clk + ((year * 365L) + day) * SECSPERDAY;
	imul	EAX,ECX,365
	add	EAX,EBX		;AX = year * 365L + day
	imul	EAX,SECSPERDAY

	pop	EDX		;get # of seconds since midnight
	add	EAX,EDX
	add	EAX,012CF0870h	;convert to time since 1970

    if SPTR
	mov	EDX,P[EBP]
	tst	EDX
	jz	LAB		;if timer is NULL
	mov	[EDX],EAX	; *timer = t;
    else
	mov	EDX,P[EBP]
	movzx	ECX,word ptr P+4[EBP]
	or	ECX,EDX
	jz	LAB		;if timer is NULL
	mov	ES,P+4[EBP]
	mov	ES:[EDX],EAX	; *timer = t;
    endif
LAB:
	unuse	<EBX>
	pop	EBP
	ret
c_endp	time

	endcode	time2

endif ;Atime2

ifdef Autime

; variables used by utime
t		equ	-4
varsize		equ	4

	begdata

	extrn	_errno:dword

	enddata

    if LCODE
	extrn	_time:far, _localtime:far
    else
	extrn	_time:near, _localtime:near
    endif

	begcode	utime

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set the time stamp on a file.
;	int utime(char *filespec,time_t timep[2]);
; Input:
;	filespec ->	ascii string giving the path and filename
;	timep ->	a 'last accessed' time and 'last modified' time,
;			respectively. MS-DOS has no concept of 'last
;			accessed' time, so that field is ignored, and the
;			time stamp is set to the 'last modified' time.
;			If timep is NULL, the current time is used.
; Returns:
;	0	success
;	-1	failure, errno will have a clue

	c_public utime
func	utime
	push	EBP
	mov	EBP,ESP
	sub	ESP,varsize
	uses	<EBX>

    if LPTR
	movzx	EBX,word ptr P+SIZEPTR+4[EBP]
	or	EBX,P+SIZEPTR[EBP]
	jz	L2
	les	EBX,P+SIZEPTR[EBP]
	jz	L2
	mov	EAX,ES:4[EBX]	;t = timep[1]; get 'last modified' time
    else
	mov	EBX,P+SIZEPTR[EBP]
	or	EBX,EBX
	jz	L2
	mov	EAX,4[EBX]	;t = timep[1]; get 'last modified' time
    endif
	jmps	L3
L2:
    if LPTR
	push	EBX
    endif
	push	EBX		;push NULL
	call	_time
	add	ESP,SIZEPTR
L3:
	mov	t[EBP],EAX

	;bd = localtime(&t);
    if LPTR
	push	SS
    endif
	lea	EBX,t[EBP]
	push	EBX
	call	_localtime	;localtime(&t)
	add	ESP,SIZEPTR

	mov	EBX,EAX
    if LPTR
	push	DS
	mov	DS,EDX
    endif

	;date = ((((bd->tm_year - 80) << 4) + bd->tm_mon + 1) << 5) +
	;	bd->tm_mday;

	mov	EAX,tm_year[EBX]
	sub	EAX,80
	shl	EAX,4
	add	EAX,tm_mon[EBX]
	inc	EAX
	shl	EAX,5
	add	EAX,tm_mday[EBX]
	mov	EDX,EAX

	;tday = (((bd->tm_hour << 6) + bd->tm_min) << 5) + (bd->tm_sec >> 1);
	mov	EAX,tm_hour[EBX]
	shl	EAX,6
	add	EAX,tm_min[EBX]
	shl	EAX,5
	mov	ECX,tm_sec[EBX]
	shr	ECX,1
	add	ECX,EAX
	push	EDX
	mov	EDX,P[EBP]
	mov	EAX,03D00h		;open file for reading
    if LPTR
	mov	DS,P+4[EBP]
    endif
	bdos
	pop	EDX
    if LPTR
	pop	DS
    endif
	jc	err

	mov	EBX,EAX		;handle
;	mov	ECX,tday
;	mov	EDX,date
	mov	EAX,05701h	;set date/time
	bdos
	jc	err

	bdos	3Eh		;close file
	jc	err

	clr	EAX
L1:
	unuse	<EBX>
	mov	ESP,EBP
	pop	EBP
	ret

err:	movzx	EAX,AX
	mov	DGROUP:_errno,EAX
	bdos	3Eh		;close file
	mov	EAX,-1
	jmp	L1
c_endp	utime

	endcode	utime

endif ;Autime

	end
