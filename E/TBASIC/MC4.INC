'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC4.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: Load, Save and print a spreadsheet. Display on-line manual   ³
'³              and DOS accces.						     ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

DEF FNExists%(FileName$)
' The function Exists% returns a non zero integer value
' If the file specified by FileName$ is on the current disk drive.

    LOCAL ExistF%
    ON ERROR GOTO FileError             ' if error occurs then trap it at
                                        ' FileError
    ExistF% = %TRUE                     ' Initial value of FNexists%
    OPEN FileName$ FOR INPUT AS #9      ' Trying to open file
    IF ERR=0 THEN CLOSE #9              ' If no error occurs we need to close
    GOTO Finish				' file

    FileError:
      ExistF% = %FALSE                  ' File doesn't exist
      RESUME NEXT

    Finish:
    ON ERROR GOTO 0
    FNExists%=ExistF%

END DEF

SUB GetFileName(FileName$,FileNameOk%)
' GetFileName prompts the user for a filename and reads it

    FileName$=""
    CALL GetLine(POS(0), CSRLIN, 12, UpperCase%, ErrorPosition%, FileName$)
    IF FileName$="" THEN
       FileNameOk%=%False
    ELSEIF INSTR(FileName$,".")>9 THEN
       FileNameOk%=%False
    ELSEIF  INSTR(FileName$,".")=0 AND LEN(FileName$)>8 THEN
       FileNameOk%=%False
    ELSE
       ON ERROR GOTO ErrOpenFile
       OPEN "R",#9,FileName$,1 : Fsize=LOF(9) : CLOSE 9 : FileNameOk%=%True
       IF Fsize=0 THEN KILL FileName$
    END IF

    FinishOpenFile:
    ON ERROR GOTO 0
    EXIT SUB

   ErrOpenFile:
       CLOSE 9 : FileNameOk%=%False
       RESUME  FinishOpenFile

END SUB

SUB save
' Save the SpreadSheet into file

  SHARED SpreadSheet%(),Globfx%,Globfy%,AutoCalc%,Border%
  LOCAL FileName$,Byte$

    CALL NormVideo
    DO
       CALL Msg( "Enter the SpreadSheet save name : " )
       CALL GetFileName( FileName$, FileNameOk%)
       IF FileNameOk%<>%True  THEN BEEP
    LOOP UNTIL FileName$="" OR FileNameOk%=%True  OR Filename$=CHR$(255)
    IF FileName$<>CHR$(255) AND FileName$<>"" THEN
       CALL Msg("Saving File "+UCASE$(Filename$)+" ...")
       DEF SEG=VAL("&H"+HEX$(FNUnsign&(varseg(SpreadSheet%(%FxMin ,%FyMin ,1)))))
       BSAVE FileName$,FNUnsign&(varptr(SpreadSheet%(%FxMin ,%FyMin ,1)))_
             ,%SheetSize
       DEF SEG
       OPEN FileName$ FOR BINARY AS #1
       SEEK #1,LOF(1)
       IF AutoCalc%<0 THEN Byte$=CHR$(0) ELSE Byte$=CHR$(1)  ' to prevent
       PUT$ #1,Byte$                                         ' negative value
       IF Border%<0 THEN Byte$=CHR$(0) ELSE Byte$=CHR$(1)    '
       PUT$ #1,Byte$                                         '
       CLOSE #1
    END IF

    CALL GotoCell(GlobFx%,GlobFy%)

END SUB

SUB load
' load a spreadsheet

  SHARED SpreadSheet%(),GlobFx%,GlobFy%,AutoCalc%,Border%,FileName$
  LOCAL Byte$, Ofs%

    IF FileName$="" THEN
       CALL NormVideo
       DO
          CALL Msg( "Enter the SpreadSheet Name TO load : " )
          CALL GetFileName( FileName$, FileNameOk%)
          IF FileNameOk%=%True  THEN FileNameOk%=FNexists%(FileName$)
          IF FileNameOk%<>%True  THEN BEEP
       LOOP UNTIL FileName$="" OR FileNameOk%=%True  OR Filename$=CHR$(255)
    END IF

    IF FileName$<>CHR$(255) AND FileName$<>"" THEN
      CALL Msg("Loading File "+UCASE$(Filename$)+" ...")
      DEF SEG = VAL("&H"+HEX$(FNUnsign&(varseg(SpreadSheet%(%FxMin ,%FyMin ,1)))))
      BLOAD FileName$,FNUnsign&(varptr(SpreadSheet%(%FxMin ,%FyMin ,1)))
      DEF SEG
      OPEN FileName$ FOR BINARY AS #1
      SEEK #1,LOF(1)-2
      GET$ #1,1,Byte$
      AutoCalc%=ASC(Byte$)
      GET$ #1,1,Byte$
      Border%=ASC(Byte$)
      CLOSE #1
      IF AutoCalc%=0 THEN AutoCalc%=-2         ' to retrieve a boolean value
      IF Border%=0 THEN Border%=-2
      GlobFx% = %FXMin
      GlobFy% = %FYMin
      CALL UpDate
    END IF

    CALL GotoCell(GlobFx%,GlobFy% )

END SUB

SUB PrintSheet
' print sheet to the printer (or to a file)

  SHARED Enter$,SpreadSheet%(),GlobFx%,GlobFy%,Xpos%()

  LOCAL I%, J%, FX%, FY%, Contents$, M$
  LOCAL Value#, Dec%, FW%, FileName$
  LOCAL CurrLine$, TabCount%, CellStatus%

    DO
       CALL Msg( "Enter filename or "+Enter$+" for Printer : " )
       CALL GetFileName( FileName$, FileNameOk%)
       IF FileNameOk%<>%True  AND FileName$<>"" THEN BEEP
    LOOP UNTIL FileName$="" OR FileNameOk%=%True  OR FileName$=CHR$(255)
    IF FileName$<>CHR$(255) THEN
       IF FileName$=""  THEN
         FileName$ = "LPT1:"
       END IF
       CALL Msg( "Left Margin : " )
       CALL GetLine(POS(0), CSRLIN, 3, UpperCase%, ErrorPosition%, M$)
       LeftMargin%=abs(VAL(M$))
       CALL BlinkVideo
       CALL Msg( "  Printing to " + FileName$ + " ..." )
       CALL NormVideo
       ON ERROR GOTO PRError
       OPEN FileName$ FOR output AS #1
       FOR I% = 1 TO 2
          PRINT# 1,""
       NEXT I%
       FOR J% = %FYMin  TO %FYMax
        CurrLine$ = ""
        FOR I% = %FXMin  TO %FXMax
           CALL GetRec( I%,J%,CellStatus%,Contents$,Value#,Dec%,FW%,CellColor% )
           WHILE LEN(CurrLine$) < XPos%(I%)
             CurrLine$ = CurrLine$ + " "
           WEND
           IF FNIN%( %Constant , CellStatus% ) THEN
                        IF NOT ( FNIN%( %Locked , CellStatus% )) THEN
               CurrLine$ = CurrLine$ + STR$(Value#)
             END IF
           ELSE
             CurrLine$ = CurrLine$ + Contents$
           END IF
        NEXT I%
        PRINT# 1,SPACE$(LeftMargin%)+CurrLine$
       NEXT J%
       CLOSE# 1
    END IF
    CALL Grid
    CALL GotoCell( GlobFX%, GlobFY% )
    ON ERROR GOTO 0
    EXIT SUB

PRError:
    I%=%FxMax : J%=%FyMax
    RESUME NEXT

END SUB

SUB HELP
' on-line Help

  LOCAL L$,J,Bold,Bold$,Revers,Revers$
  SHARED Enter$

  Bold$=CHR$(2) : Ch$="" : Revers$=CHR$(9)
  IF FNExists%("MC.HLP") THEN
    OPEN "MC.HLP" FOR INPUT AS #9
    WHILE NOT eof(9) AND UCASE$(Ch$)<>"Q"
      Bold=%False  : Revers=%False
      CALL LowVideo
      CLS
      LINE INPUT#9,L$
      DO
        PRINT "     ";
        FOR J=1 TO LEN(L$)
          IF MID$(L$,J,1)=Bold$ THEN
             Bold=NOT Bold
             IF Bold THEN CALL NormVideo ELSE CALL LowVideo
          ELSEIF MID$(L$,J,1)=Revers$ THEN
             Revers=NOT Revers
             IF Revers THEN COLOR 0,7 ELSE CALL LowVideo
          ELSE
             PRINT MID$(L$,J,1);
          END IF
        NEXT
        PRINT
        LINE INPUT#9,L$
      LOOP UNTIL  eof(9) OR LEFT$(L$,3)=".PA"
      CALL NormVideo : LOCATE 23,12
      PRINT "--- Please Strike ";
      COLOR 9,0 : PRINT "Q"; : CALL NormVideo
      PRINT " To Quit help or any key to continue ---"
      CALL LowVideo
      CALL ReadKBD(Ch$)
    WEND
    CLOSE #9
    IF UCASE$(Ch$)<>"Q" THEN
       CALL NormVideo : LOCATE 23,13 : CALL ClrEol : BEEP
       PRINT "-- Please strike ";
       COLOR 9,0 : PRINT ENTER$; : CALL Normvideo
       PRINT " TO start MicroCalc ---";
       CALL LowVideo
       DO
         CALL ReadKBD(Ch$)
       LOOP UNTIL Ch$=CHR$(13)
    END IF
  ELSE
    PLAY "CDC"
    CALL Msg("   To get help the file MC.HLP must be on your disk. Strike "_
    +ENTER$+" to continue")
    DO
      CALL ReadKBD(Ch$)
    LOOP UNTIL Ch$=CHR$(13)
  END IF

END SUB

SUB DosShell
' execute a DOS command

    SHARED Enter$
    LOCAL Dos$,Ch$

    CALL NormVideo
    CALL Msg( "Enter a DOS command or "+Enter$+" to go to DOS : ")
    CALL Getline(POS(0),CSRLIN,79-POS(0),%False ,0,Dos$)
    IF Dos$<>CHR$(255) THEN
       CLS
       PRINT "MicroCalc Version 1.00A - Dos Shell"
       IF Dos$="" THEN
          PRINT
          COLOR 4,15
          PRINT " Type EXIT and "+Enter$+" to return to MicroCalc "
          COLOR 14,0
       END IF
       Shell Dos$
       IF Dos$<>"" THEN
          LOCATE 24,1
          PRINT
          PRINT "Strike any key to come back to MicroCalc ...";
          CALL ReadKbd(Ch$)
       END IF
    END IF

END SUB

