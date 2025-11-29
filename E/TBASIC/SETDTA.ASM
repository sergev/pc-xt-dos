
; This routine  expects three  routines to be passed onto the stack.  The
; first  and second  represent the possible address(Segment and Offset) of
; the new DTA while the third is a string that may have been allocated for
; the DTA.  If  both the Segment  and Offset values are equal to zero then
; the  DTA will be set to  the location of the string descriptor. However,
; if either  the segment or the  offset is not equal to zero  then the DTA
; will be set to the address specified by the first two parameters.
;
;  	STACK after saving BP
;
;   ³ 32 Bit(Segment and Offset) ³
;   ³ pointer to TB integer rep- ³ <----- BP + 0Eh
;   ³ resenting segment of DTA   ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³ 32 Bit(Segment and Offset) ³
;   ³ pointer to TB integer rep- ³ <----- BP + 0Ah
;   ³ resenting offset of DTA	 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³ 32 Bit(Segment and Offset) ³
;   ³ pointer to TB string desc- ³ <----- BP + 6
;   ³ riptor representing DTA    ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³     return address that	 ³
;   ³     TB will go to after	 ³ <----- BP + 2
;   ³    completion of routine	 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³    saved BP          	 ³ <----- BP
;   ³                      	 ³
;   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

Zero     equ  00h	; equates for this program
DosCall  equ  21h
SetDTA   equ  1Ah


program segment  ; begin program segment
  assume cs:program

  push  bp      ; save bp
  mov   bp, sp
  push  es      ; save es because we'll use it for pointer manipulation
  push  ds      ; ditto

  les   di, [bp + 0Eh]  ; get pointer to segment
  mov   ax, es:[di]     ; put segment value in ax
  cmp   ax, Zero
  jnz   SegOrOfsIsNotZero  ; if ax <> 0 then use segment and offset values

  les   di, [bp + 0Ah]  ; get pointer to segment
  mov   dx, es:[di]     ; put segment value in ax
  cmp   dx, Zero
  jz    SegAndOfsZero   ; if ax = 0 then use string location

SegOrOfsIsNotZero:
  mov   ds, ax
  mov   si, dx

SegAndOfsZero:
  ; need to get location of string
  les   di, [bp + 6h] ; get pointer to Turbo Basic String Descriptor
  mov   dx, ds:[0]    ; get the beginning of the string segment from ds:[0]
  push  dx
  pop   ds            ; make ds point to string segment
  mov   dx, es:[di + 2]   ; get offset into string segment from es:[di + 2]

DoDOSCall:
  ; now ds:dx points to the appropriate location for the DOS Set DTA
  ; call
  mov   ah, SetDTA
  int   DosCall

  pop   ds
  pop   es      ; restore registers
  pop   bp

program ends	; end program segment

end

