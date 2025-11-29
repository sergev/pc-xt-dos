
; This  routine expects two pointers to be passed on the stack.  The first
; points to  a Turbo Basic integer which indicates which drive to look at.
; The second is a pointer to a Turbo Basic string descriptor which in turn
; points to the string which will store the directory returned by DOS. The
; string must have been allocates 64 characters in length or this  routine
; will cause string memory to become corrupt.
;
;  	STACK after saving BP
;   ³ 32 bit(segment and offset) ³
;   ³ to TB integer representing ³ <----- BP + 0Ah
;   ³ which drive to look at     ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³ 32 bit(segment and offset) ³
;   ³ to TB string representing  ³ <----- BP + 6
;   ³ current path 		 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³    return address that  	 ³
;   ³    TB will go to after  	 ³ <----- BP + 2
;   ³   completion of routine	 ³
;   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;   ³         saved BP         	 ³ <----- BP
;   ³                      	 ³
;   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


DosCall  equ  21h	; equates for assembler routine
GetDir   equ  47h

program segment	; begin program segment
  assume cs:program

  push  bp      ; save bp
  mov   bp, sp
  push  es      ; save es because we'll use it for pointer manipulation
  push  ds      ; ditto

  les   di, [bp + 6h]	; load pointer to string descriptor into es:di
  mov   dx, ds:[0]	; get the beginning of the string segment from ds:[0]
  push  dx
  pop   ds		; make ds point to string segment
  mov   si, es:[di + 2]	; get offset into string segment from es:[di + 2]

  les   di, [bp + 0Ah] ; load pointer to TB integer into es:di
  mov   dx, es:[di]    ; load which drive to look at into dx

  ; now ds:si points to the appropriate location for the DOS Get Directory
  ; call and dl contains the drive to look at
  mov   ah, GetDir
  int   DosCall

  pop   ds
  pop   es      ; restore registers
  pop   bp

program ends	; end program segment

end

