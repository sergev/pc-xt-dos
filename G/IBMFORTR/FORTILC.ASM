           PAGE       60,132
           TITLE      FORTRAN/2 to other languages interface routines
           NAME       FORTILC
;
; This version of FORTILC is for use with FORTRAN/2 version 1.00
;
; This routine supports calling assembly language SUBROUTINEs that
; were orginally coded to be CALLed from IBM FORTRAN.
;
; It also supports the invocation of assembly language
; FUNCTIONs of the following types: LOGICAL, LOGICAL*1, LOGICAL*2,
; LOGICAL*4, INTEGER*2, INTEGER*4. REAL(REAL*4),
; DOUBLE PRECISION(REAL*8),COMPLEX(COMPLEX*8) and
; COMPLEX*16.
;
; The calling sequence is:
;
;     CALL FORTILC(ASM_NAME,ARG_1, . . . ,ARG_N,ARG_COUNT)
; or:
;     L_VAR = FUNCILC(ASM_NAME,ARG_1, . . . ,ARG_N,ARG_COUNT)
;
; where:
;
; L_VAR is the name of a variable of the appropriate type.
; FUNCILC is the name of the typed function within this assembly.
; ASM_NAME is the name of the assembly language routine.
; ARG_COUNT is the number of arguments in the list.
; ARG_i is an argument to the assembler routine.
;
; Note 1:
; ASM_NAME must appear in an EXTERNAL statement.
;
; Note 2:
; If the function form is used, the name must be typed,
; i.e., you must declare (for example) INTEGER*2 FUNCI2 or
; LOGICAL FUNCL.
;
; Note 3:
; This routine does not support alternate returns as arguments.
;
; Note 4:
; This routine does not support arguments which will be referenced as
; CHARACTER*(*). To pass other CHARACTER arguments you must declare the
; function name as OS EXTERNAL.
;
;     e.g.  OS EXTERNAL FUNCI2
;           OS EXTERNAL FORTILC
;
; Note 5:
; Dynamic calls may be supported as follows:
;
; The first parameter ASM_NAME may be replaced by $VAL of an
; INTEGER*4 parameter containing the address of the routine
; to be called.
;
; The calling sequence becomes:
;
;     CALL FORTILC($VAL(ADDRESS),ARG_COUNT,ARG_1, . . . ,ARG_N)
; or:
;     L_VAR = FUNCILC($VAL(ADDRESS),ARG_COUNT,ARG_1, . . . ,ARG_N)
;
; where:
;
; ADDRESS is an INTEGER*4 variable to which an address in
; selector:offset format has been assigned. The address must be
; the address of an executable routine, obtained from an API or
; an assembly language routine.
; L_VAR,FUNCILC,ARG_COUNT and ARG_i are as before.
;
; Note 6:
; The code here is for the version that uses the math co-processor.
;
; To run without the math co-processor, the following edit should
; be made to the file:
;
;     change all occurrences of the string ;$E$ to 4 spaces
;
           PAGE
;
;$E$       EXTRN      F@EMUL:FAR
;
DATA       SEGMENT    WORD  PUBLIC 'F@DATA'
;
RET_ADR    EQU        THIS DWORD
RET_OFF    DW         ?
RET_SEG    DW         ?
OLD_BP     DW         ?
RESULT     DD         ?
RESULT2    DD         ?
RESULT3    DQ         ?
;
DATA       ENDS
;
F@ICODE    SEGMENT    WORD  PUBLIC  'CODE'
F@FORTILC  PROC       FAR
;
           ASSUME     CS:F@ICODE,DS:DATA,SS:NOTHING,ES:NOTHING
;
           PUBLIC     FORTILC
           PUBLIC     FUNCL
           PUBLIC     FUNCL1
           PUBLIC     FUNCL2
           PUBLIC     FUNCL4
           PUBLIC     FUNCI2
           PUBLIC     FUNCI4
           PUBLIC     FUNCR
           PUBLIC     FUNCR4
           PUBLIC     FUNCD
           PUBLIC     FUNCR8
           PUBLIC     FUNCC
           PUBLIC     FUNCC8
           PUBLIC     FUNCDC
           PUBLIC     FUNCCD
;
FORTILC    LABEL      FAR
FUNCL      LABEL      FAR
FUNCL1     LABEL      FAR
FUNCL2     LABEL      FAR
FUNCL4     LABEL      FAR
FUNCI2     LABEL      FAR
FUNCI4     LABEL      FAR
           MOV        BX,DATA
           MOV        DS,BX
           POP        RET_OFF                  ; SAVE RETURN ADDRESS
           POP        RET_SEG
           MOV        OLD_BP,BP                ; SAVE BP
           POP        BX
           POP        ES
           MOV        SI,ES:[BX]               ; NO OF ARGUMENTS
           SHL        SI,1
           SHL        SI,1
           MOV        BP,SP
           CALL       DWORD PTR SS:[BP+SI]     ; CALL ASSEMBLER ROUTINE
RETURN_TO_CALLER      LABEL NEAR
           ADD        SP,4
           MOV        BP,OLD_BP                ;RESTORE BP
           JMP        RET_ADR
;
FUNCR      LABEL      FAR
FUNCR4     LABEL      FAR
           MOV        DI,OFFSET R4_RESULT      ;
           JMP        SHORT DO_IT
;
FUNCD      LABEL      FAR
FUNCR8     LABEL      FAR
           MOV        DI,OFFSET R8_RESULT      ;
           JMP        SHORT DO_IT
;
FUNCC      LABEL      FAR
FUNCC8     LABEL      FAR
           MOV        DI,OFFSET C8_RESULT      ;
           JMP        SHORT DO_IT
;
FUNCDC     LABEL      FAR
FUNCCD     LABEL      FAR
           MOV        DI,OFFSET C16_RESULT     ;
;
DO_IT:     MOV        BX,DATA
           MOV        DS,BX
           POP        RET_OFF                  ; SAVE RETURN ADDRESS
           POP        RET_SEG
           MOV        OLD_BP,BP                ; SAVE BP
           POP        BX
           POP        ES
           MOV        SI,ES:[BX]               ; NO OF ARGUMENTS
           SHL        SI,1
           SHL        SI,1
           MOV        BP,SP
           MOV        BX,OFFSET RESULT
           PUSH       BX                       ; PASS RESULT ADDRESS
           PUSH       CS                       ; SETUP STACK TO RETURN TO
           PUSH       DI                       ; ADDRESS FOR THE RESULT TYPE.
           JMP        DWORD PTR SS:[BP+SI]
;
R4_RESULT  LABEL      NEAR
;$E$       CALL       F@EMUL
           FLD        RESULT                   ; LOAD REAL RESULT
           FWAIT
           JMP        RETURN_TO_CALLER         ; RETURN TO FORTRAN CALLER
;
R8_RESULT  LABEL      NEAR
;$E$       CALL       F@EMUL
           FLD        QWORD PTR RESULT         ; LOAD REAL RESULT
           FWAIT
           JMP        RETURN_TO_CALLER         ; RETURN TO FORTRAN CALLER
;
C8_RESULT LABEL NEAR
;$E$       CALL       F@EMUL
           FLD        RESULT2                  ; LOAD IMAG PART OF RESULT
           FLD        RESULT                   ; LOAD REAL PART OF RESULT
           FWAIT
           JMP        RETURN_TO_CALLER         ; RETURN TO FORTRAN CALLER
;
C16_RESULT LABEL      NEAR
;$E$       CALL       F@EMUL
           FLD        RESULT3                  ; LOAD IMAG PART OF RESULT
           FLD        QWORD PTR RESULT         ; LOAD REAL PART OF RESULT
           FWAIT
           JMP        RETURN_TO_CALLER         ; RETURN TO FORTRAN CALLER
;
F@FORTILC  ENDP
F@ICODE    ENDS
;
STACK      SEGMENT    WORD    STACK    'STACK'
STACK      ENDS
;
           END
