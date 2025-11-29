
; This  routine expects two pointers to be passed on  the stack that point
; to Turbo Basic integers.   The integers will be assigned the segment and
; offset of the current DTA.
;
;  STACK after saving BP
;
;   ³ 32 bit(segment and offset) ³
;   ³ pointer to TB integer rep- ³ <----- BP + 0Ah
;   ³ resenting segment of DTA	 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³ 32 bit(segment and offset) ³
;   ³ pointer to TB integer rep- ³ <----- BP + 6
;   ³ resenting offset of DTA	 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³   return address that TB	 ³
;   ³   will go to after comple- ³ <----- BP + 2
;   ³   tion of routine		 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³         saved BP		 ³ <----- BP
;   ³				 ³
;   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


DosCall  equ  21h	; equates for this module
GetDTA   equ  2Fh

program segment	; begin program segment
  assume cs:program

  push  bp      ; save bp
  mov   bp, sp
  push  es      ; save es because we'll use it for pointer manipulation
  push  ds      ; ditto

  ; call dos function to get current DTA's location
  mov   ah, GetDTA
  int   DosCall

  ; now get pointers to Turbo Basic integers and store segment and
  ; offset of DTA in them

  lds   di, [bp + 6h]	; load pointer to offset variable
  mov   ds:[di], bx	; mov offset of DTA into Turbo Basic integer

  lds   di, [bp + 0Ah]  ; load pointer to segment variable
  mov   ds:[di], es	; mov offset of DTA into Turbo Basic integer

  pop   ds
  pop   es	; restore registers
  pop   bp

program ends	; end program segment

end

