         IF1
$SVSN    MACRO
         MOV       $PPTR,0
         CALL      $SVP
         ENDM
$SVSF    MACRO
         MOV       $PPTR,1
         CALL      $SVP
         ENDM
$SVSC    MACRO
         MOV       $PPTR,2
         CALL      $SVP
         ENDM
$SVSH    MACRO
         MOV       $PPTR,3
         CALL      $SVP
         ENDM
$SVRT    MACRO
         MOV       $PPTR,4
         CALL      $SVP
         ENDM
$SVSA    MACRO
         MOV       $PPTR,5
         CALL      $SVP
         ENDM
$SVGA    MACRO
         MOV       $PPTR,6
         CALL      $SVP
         ENDM
$SVPR    MACRO
         MOV       $PPTR,7
         CALL      $SVP
         ENDM
$SVPW    MACRO
         MOV       $PPTR,8
         CALL      $SVP
         ENDM
$SVSZ    MACRO
         MOV       $PPTR,9
         CALL      $SVP
         ENDM
$SVRD    MACRO
         MOV       $PPTR,10
         CALL      $SVP
         ENDM
$SVCP    MACRO
         MOV       $PPTR,11
         CALL      $SVP
         ENDM
$SVWR    MACRO
         MOV       $PPTR,12
         CALL      $SVP
         ENDM
$SVRL    MACRO
         MOV       $PPTR,13
         CALL      $SVP
         ENDM
$SVWA    MACRO
         MOV       $PPTR,14
         CALL      $SVP
         ENDM
$SVSV    MACRO
         MOV       $PPTR,15
         CALL      $SVP
         ENDM
$PAPL    MACRO
$P1      EQU       WORD PTR [BP]
$P2      EQU       WORD PTR [BP+2]
$P3      EQU       WORD PTR [BP+4]
$P4      EQU       WORD PTR [BP+6]
$P5      EQU       WORD PTR [BP+8]
$P6      EQU       WORD PTR [BP+10]
$PPTR    EQU       WORD PTR [BP+8]
$PLKR    EQU       WORD PTR [BP+10]
         ENDM
$SAVE    MACRO
         PUSH      AX
         PUSH      BX
         PUSH      BP
         PUSH      $P6
         PUSH      $P5
         PUSH      $P4
         PUSH      $P3
         PUSH      $P2
         PUSH      $P1
         MOV       BP,SP
         ENDM
         ENDIF
$SQ      SEGMENT   COMMON
$WBEG1   LABEL     BYTE
$SVP     EQU       DWORD PTR $WBEG1+371H   ;SVP LINK
$APLOFF  EQU       $WBEG1+4ACH             ;ASCII TO APL T.TABLE
$APLON   EQU       $WBEG1+5ACH             ;APLON KEYBOARD TABLE
$ASCII   EQU       $WBEG1+6ACH             ;APL TO ASCII T.TABLE
$WSW     EQU       $WBEG1+7ACH             ;SWITCHES
$WBUF    EQU       $WBEG1+7CEH             ;APL I/O BUFFER
$PMR     EQU       WORD PTR $WBEG1+8ACH    ;APLDFN START ADDRESS
$L       EQU       $PMR+52*TYPE $PMR       ;INTERPRETER WORK AREA
                                           ;DO NOT USE IT.
$IFLOT   EQU       $L+56*TYPE $L           ;RETURN CODES WORD
$SQ      ENDS
