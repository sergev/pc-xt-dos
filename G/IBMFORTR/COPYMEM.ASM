           PAGE       60,132
           TITLE      FORTRAN/2 COPY MEMORY ROUTINE
           NAME       CMEMPAA1
;
; This version of COPY TO/FROM MEMORY is for use with FORTRAN/2 version 1.00
;
; The module has two entry points: COPYTOMEMORY and  COPYFROMMEMORY
;
; The subroutines allow data to be copied into and out of addressed memory
; The memory address, in selector:offset format is obtained from an assembly
; language routine or from an API and placed in an INTEGER*4 variable
;
; A typical calling sequence is:
;
;      External CopyToMemory,CopyFromMemory
;      Integer*4 Address
;      Integer*2 Selector,Offset,SelectorOffset(2)
;      Equivalence (Address,Offset,SelectorOffset(1))
;      Equivalence (Selector,SelectorOffset(2))
;      Character*20 CharOut, CharIn
;       ...
;C     Address = ... memory address...
;
;C     CharOut = ' ... data ...'
;      Call CopyToMemory(Address,CharOut)
;       ...
;      Call CopyFromMemory(Address,CharIn)
;      ...
;
; Address is the INTEGER*4 variable containing the memory address.
; In the example, equivalence statements are used to enable manipulation
; of the Selector and Offset components of Address if required.
;
; CharIn is the character variable in to which the memory is copied.
; It could also be a character array element, character substring,
; or character array element substring. The length of CharIn determines
; the number of bytes copied from memory into CharIn.
;
; CharOut is the data to be copied to memory. It can also be a character
; expression. The length of CharOut determines the number of bytes copied
; into memory.
;
; Equivalence can be used to copy other types of data into and out of
; memory. The following shows an example of placing Integer*2 data and
; Real*8 data in memory.
;
;      External CopyToMemory
;      Integer*4 Address
;
;      Character IDataChar*2,RDataChar*8
;      Integer*2 IData
;      Real*8 RData
;      Equivalence (IData,IDataChar),(RData,RDataChar)
;       ...
;C     Address = ...memory address...
;      IData = 4567
;      Call CopyToMemory(Address,IDataChar)
;       ...
;C     Address = ...some other address in memory...
;      RData = 23.34D-34
;      Call CopyToMemory(Address,RDataChar)
;       ...
           PAGE
           EXTRN      F@GTSC:FAR
DATA       SEGMENT    WORD  PUBLIC  'F@DATA'
;
; COPY_MEMORY_DATA IS A STRUCTURE REPRESENTING THE STATE OF
; THE 8086 STACK AFTER BP HAS BEEN SAVED AND THE ARGUMENT LIST
; HAS BEEN MADE ADDRESSABLE
;
COPY_MEMORY_DATA              STRUC
                              DW        ?
                              DD        ?
CHAR_LENGTH                   DW        ?
CHAR_OFFSET                   DW        ?
CHAR_SEGMENT                  DW        ?
ADDRESS_OFFSET                DW        ?
ADDRESS_SEGMENT               DW        ?
COPY_MEMORY_DATA              ENDS
;
CHAR_ADDRESS                  EQU       DWORD PTR CHAR_OFFSET
ADDRESS_ADDRESS               EQU       DWORD PTR ADDRESS_OFFSET
;
SEGMENT_INCREMENT             DW        0
;
DATA       ENDS
;
           PAGE
F@ICODE    SEGMENT    WORD  PUBLIC  'CODE'
;
           ASSUME     CS:F@ICODE,DS:DATA,ES:NOTHING,SS:NOTHING
;
           PUBLIC     COPYTOMEMORY
           PUBLIC     COPYFROMMEMORY
;
    PAGE
COPYTOMEMORY          PROC   FAR
;
; ON ENTRY THE ARGUMENT LIST IS ON THE STACK
;         1ST ARGUMENT IS ADDRESS OF I*4 ITEM HOLDING ADDRESS
;                   OF THE MEMORY BUFFER
;         2ND ARGUMENT IS ADDRESS AND LENGTH
;                   FOR THE CHARACTER ITEM TO BE COPIED FROM MEMORY
;
           PUSH      BP
           MOV       BP,SP
           CALL      COPY_COMMON
           LDS       SI,[BP].CHAR_ADDRESS
           LES       DI,[BP].ADDRESS_ADDRESS
           REP       MOVSB
           CMP       [BP].CHAR_LENGTH,0          ; IF CHARACTER ITEM IS MULTI
           JE        RETURN_TO_CALLER            ; SEGMENT THEN PERFORM THE
           MOV       AX,DATA                     ; COPY IN TWO PARTS.
           MOV       DS,AX
           MOV       AX,SEGMENT_INCREMENT
           ADD       [BP].CHAR_SEGMENT,AX
           MOV       [BP].CHAR_OFFSET,0
           LDS       SI,[BP].CHAR_ADDRESS
           MOV       CX,[BP].CHAR_LENGTH
           REP       MOVSB
RETURN_TO_CALLER:
           POP       BP
           RET       10
COPYTOMEMORY         ENDP
    PAGE
COPYFROMMEMORY       PROC   FAR
;
; ON ENTRY THE ARGUMENT LIST IS ON THE STACK
;         1ST ARGUMENT IS ADDRESS OF I*4 ITEM HOLDING ADDRESS
;                   OF THE MEMORY BUFFER
;         2ND ARGUMENT IS ADDRESS AND LENGTH
;                   FOR THE CHARACTER ITEM TO BE FILLED FROM MEMORY
;
           PUSH      BP
           MOV       BP,SP
           CALL      COPY_COMMON
           LDS       SI,[BP].ADDRESS_ADDRESS
           LES       DI,[BP].CHAR_ADDRESS
           REP       MOVSB
           CMP       [BP].CHAR_LENGTH,0          ; IF CHARACTER ITEM IS MULTI
           JE        RETURN_TO_CALLER            ; SEGMENT THEN PERFORM THE
           MOV       AX,DATA                     ; COPY IN TWO PARTS.
           MOV       DS,AX
           MOV       AX,SEGMENT_INCREMENT
           ADD       [BP].CHAR_SEGMENT,AX
           MOV       [BP].CHAR_OFFSET,0
           LES       DI,[BP].CHAR_ADDRESS
           MOV       DS,[BP].ADDRESS_SEGMENT
           MOV       CX,[BP].CHAR_LENGTH
           REP       MOVSB
           JMP       RETURN_TO_CALLER
COPYFROMMEMORY       ENDP
    PAGE
;
; COMMON SUBROUTINES
;
COPY_COMMON          PROC  NEAR
;
; THIS ROUTINE PERFORMS THE FOLLOWING FUNCTIONS
;
;   1) SETS UP ADDRESSING OF LOCAL DATA
;
;   2) SETS UP SEGMENT INCREMENT
;
;   3) CALCULATES THE LENGTH OF THE FIRST PART OF THE COPY.
;      IF THE CHARACTER ITEM IS SINGLE SEGMENT THE LENGTH OF THE
;      SECOND PART OF THE COPY WILL BE 0.
;
;   4) MAKES THE AREA OF MEMORY POINTED TO BY Address ADDRESSABLE.
;
; ON ENTRY THE ARGUMENT LIST IS ON THE STACK.
;
; ON EXIT  CX CONTAINS THE LENGTH OF THE FIRST PART OF THE COPY.
;
           MOV       AX,DATA                     ; MAKE LOCAL DATA ADDRESSABLE
           MOV       DS,AX
           CMP       SEGMENT_INCREMENT,0         ; SET UP SEGMENT INCREMENT
           JNE       SEGMENT_INCREMENT_CALCULATED
           CALL      F@GTSC
           MOV       CL,AL
           MOV       AX,1
           SHL       AX,CL
           MOV       SEGMENT_INCREMENT,CX
SEGMENT_INCREMENT_CALCULATED:
           MOV       CX,[BP].CHAR_OFFSET         ; CALCULATE LENGTH OF FIRST
           NEG       CX                          ; PART OF COPY
           CMP       CX,[BP].CHAR_LENGTH
           JBE       LENGTH_OF_FIRST_PART_OF_COPY_CALCULATED
           MOV       CX,[BP].CHAR_LENGTH
LENGTH_OF_FIRST_PART_OF_COPY_CALCULATED:
           SUB       [BP].CHAR_LENGTH,CX
           LES       BX,[BP].ADDRESS_ADDRESS     ; ADDRESS AREA OF MEMORY
           MOV       AX,ES:[BX]                  ; POINTED TO BY Address.
           MOV       [BP].ADDRESS_OFFSET,AX
           MOV       AX,ES:[BX+2]
           MOV       [BP].ADDRESS_SEGMENT,AX
           RET
COPY_COMMON          ENDP
F@ICODE    ENDS
;
STACK      SEGMENT   WORD  STACK  'STACK'
           DW        ?
STACK      ENDS
;
           END
