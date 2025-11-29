
; This routine expects a pointer to be  passed on the stack that points to
; a Turbo Basic integer.  The integer is assigned the value of the current
; logged drive.
;
;  STACK after saving BP
;
;   ³ 32 bit(segment offset) ³
;   ³ pointer to TB integer  ³ <----- BP + 6
;   ³ representing drive id  ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³ return address that    ³
;   ³ TB will go to after    ³ <----- BP + 2
;   ³ completion of routine  ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³       saved BP	     ³ <----- BP
;   ³			     ³
;   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ



DosCall  equ  21h	; equates for module
GetDrive equ  19h


program segment  ; begin program segment
  assume cs:program

  push  bp	; save bp
  mov   bp, sp
  push  es      ; save es because we'll use it for pointer manipulation

  les   di, [bp + 6h]	; load pointer to integer into es:di

  mov   ah, GetDrive
  int   DosCall
  cbw
  mov   es:[di], ax

  pop   es	; restore registers
  pop   bp

program ends	; end program segment

end

