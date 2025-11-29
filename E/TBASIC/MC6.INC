'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC6.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: This module contains the routines to read, update, color and ³
'³		format cells. It also contains the Commands dispatcher.      ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

SUB ClearCells(Fx%, Fy%)
  ' ClearCells clears the current cell and its associated cells.
  ' An associated cell is a cell overwritten by data from the current
  ' cell. The data can be text, in which case the cell has the attribute
  ' field "overwritten." If the data is the result of an expression and
  ' the field width is larger than 11 then the associated cell is
  ' "Locked."

  LOCAL I%,CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%
  SHARED Xpos%()
  I% = Fx%

  DO   ' clear all cells that are NOT overwritten or blocked
    LOCATE Fy% + 1, Xpos%(I%)
    PRINT "           ";
    INCR I% : IF i%>%FxMax THEN i%=%FxMax
    CALL GetRec(I%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
  LOOP UNTIL ((FNIn%(%OverWritten ,CellStatus%)<>%True ) AND _
             (FNIn%(%Locked ,CellStatus%)<>%True )) or (i%>=%FxMax)
END SUB

SUB GotoX(X%, ColNo%, LineNo%)
  ' GotoX the cursor's horizontal position
  LOCATE LineNo%, X% + ColNo% - 1
END SUB

SUB GetLine(ColNo%, LineNo%, Max%, UpperCase%, ErrorPosition%, S$)
  ' GetLine is the routine used to GET input from the user. The
  ' procedure allows editing of input and checks that the input
  ' contains legal characters.

  LOCAL OkChars$, X%, InsertOn%
  SHARED EofLine$,Numbers$,GetInt%,EditCellMode%

  IF Getint% THEN
   OkChars$=Numbers$+"-"
   S$=""
  ELSE
   FOR X% = 32 to 254  ' initialize the set of OK characters
     OkChars$ =OkChars$+CHR$(X%)
   NEXT X%
  END IF

  InsertOn% = %TRUE
  CharMov$=CHR$(5)+CHR$(24)+CHR$(19)+CHR$(4)

  CALL LowVideo
  CALL GotoX(1, ColNo%, LineNo%)
  PRINT S$;" ";
  IF ErrorPosition%<>0 THEN
    X% = ErrorPosition%
  ELSEIF LEN(S$)=1 THEN
    X%=2
  ELSE
    X%=1
  END IF
  DO
    CALL GotoX(X%, ColNo%, LineNo%)
    CALL ReadKBD(Char$)
    CALL IbmCh(Char$)
    IF UpperCase% = 1 THEN
      Char$ = ucase$(Char$)
    END IF
    SELECT CASE left$(Char$, 1)
      CASE CHR$(27)       ' ESC
        S$ = CHR$(&HFF)   ' abort editing
        Char$ = EofLine$
      CASE CHR$(9)        ' tab Right
        IF NOT ((X%>LEN(S$)) or (X%>Max%)) THEN
          INCR X%
        END IF
      CASE CHR$(15)       ' tab Left
        IF X%>1 THEN
          decr X%
        END IF
      CASE CHR$(6)                ' move cursor to end of line
        X% = LEN(S$) + 1
      CASE CHR$(1)                ' move cursor to start of line
        X% = 1
      CASE CHR$(7)        ' delete char under cursor ^G
        IF X% <= LEN(S$) THEN
          CALL Delete(S$, X%, 1)
          CALL GotoX(1, ColNo%, LineNo%)
          PRINT S$;" ";
        END IF
      CASE CHR$(8)         ' delete char left cursor
        IF (LEN(S$) > 0) AND (X% > 1) THEN
          decr X%
          CALL Delete(S$, X%, 1)
          CALL GotoX(1, ColNo%, LineNo%)
          PRINT S$;" ";
        END IF
      CASE CHR$(22)                ' toggle Insert/Overwrite
        InsertOn% = NOT InsertOn%
      CASE ELSE
        IF FNInCharSet%(Char$,OkChars$) THEN
           IF InsertOn%=%True  AND LEN(S$)<MAX% THEN
              CALL Insert(Char$, S$, X%) : INCR X%
           ELSEIF X%>LEN(S$) AND LEN(S$)<MAX% THEN
              S$=S$+Char$ : INCR X%
           ELSEIF X%<=MAX%  THEN
               MID$(S$, X%, 1) = Char$ : INCR X%
           END IF
           CALL GotoX(1,ColNo%,LineNo%) : PRINT S$;" ";
        END IF
      END SELECT
  LOOP UNTIL Char$ = EofLine$ or _
           ((EditCellMode%=%True ) AND FNInCharSet%(Char$,CharMov$))
  IF ((EditCellMode%=%True ) AND FNInCharSet%(Char$,CharMov$)) THEN
    EditCellMode%=ASC(Char$)
    Char$ = EofLine$
  END IF
  CALL NormVideo
END SUB ' END procedure GetLine

SUB GetText(Fx%, Fy%, ErrorPosition%, S$)
  ' GetText calls GetLine with the current cells X,Y positions as
  ' parameters. This means that text entering takes place directly
  ' in the cell's position on the screen.

  LOCAL LineLength%
  SHARED Xpos%(),EditCellMode%

  Linelength% = (%FxMax-Fx%+1)*11
  IF LineLength%>70 THEN LineLength%=70
  CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
  CALL GetLine(Xpos%(Fx%), Fy% + 1, LineLength%, %FALSE , ErrorPosition%, S$)
END SUB

SUB GetFormula(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, NewStatus%)
' GetFormula calls the routine GetLine to get a line from the user.
' It then calls the routine Evaluate to evaluate the formula input.

  SHARED EditCellMode%

  DO
    CALL GetLine(1, 24, 70, %TRUE , ErrorPosition%, S$)
    IF S$ <> CHR$(&HFF) THEN
      CALL Evaluate(IsForm%, S$, EvalResult#, ErrorPosition%)
      IF ErrorPosition% <> 0 THEN
        CALL Flash(14, "Formula Error", %FALSE ) : BEEP
      ELSE
        CALL Flash(14, "             ", %FALSE )
      END IF
    END IF
  LOOP UNTIL (ErrorPosition% = 0) or (S$ = CHR$(&HFF))
  IF IsForm% THEN
    CALL Addset(%Formula ,NewStatus%)
  END IF
END SUB


SUB EditCell(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, NewStatus%)
  ' EditCell loads a copy of the current cell's contents into the
  ' variable S before calling either the procedure GetText or
  ' GetFormula. In this way, no changes are actually made to the
  ' current cell.

  SHARED  EditCellMode%

  CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
  S$ = Contents$
  IF FN In%(%Txt , CellStatus%) THEN
    CALL GetText(Fx%, Fy%, ErrorPosition% , S$ )
  ELSE
    CALL GetFormula(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, NewStatus%)
  END IF
END SUB


SUB UpdateCells(Fx%, Fy%, IsForm%, ErrorPosition%, S$, EvalResult#, NewStatus%)
' UpdateCells is a bit more complicated than the previous routines.
' Basically it makes sure to tag and untag cells which have been
' over-written or cleared by data from another cell. It also updates
' the current cell with the new type and contents which are still in
' the temporary variable S$

  LOCAL I%, FLength%
  SHARED NoPutReal#

  CALL PutRec(Fx%, Fy%, -1, S$, NoPutReal#, -1, -1, -1)
  IF FN In%(%Txt , NewStatus%) THEN
    I% = Fx%
    FLength% = LEN(S$)
    DO
      IF I%<%FxMax THEN INCR I%
      FLength% = FLength% - 11
      IF FLength%>0 THEN
        CALL AddSet(%OverWritten,CellStatus%)
        CALL AddSet(%Txt,CellStatus%)
        Contents$ = ""
        CALL PutRec(I%, Fy%, CellStatus%, Contents$, NoPutReal#, -1, -1, -1)
      ELSE
        CALL GetRec(I%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
        IF FN In%(%OverWritten , CellStatus%) THEN
          CellStatus% = %Txt
          CALL PutRec(I%, Fy%, CellStatus%, CHR$(0), NoPutReal#, -1, -1,-1)
          CALL GotoCell(I%, Fy%)
          CALL LeaveCell(I%, Fy%)
        END IF
      END IF
      CALL GetRec(I%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
    LOOP UNTIL (I% = %FxMax ) or (Contents$ <> "")
    CellStatus% = %Txt
    CALL PutRec(Fx%, Fy%, CellStatus%, CHR$(0), NoPutReal#, -1, -1,-1)
  ELSE ' string changed to formula or constant

    I% = Fx%
    DO
      CALL GetRec(I%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
      IF FN In%(%OverWritten , CellStatus%) THEN
        CellStatus% = %Txt
        Contents$ = ""
        CALL PutRec(I%, Fy%, CellStatus%, Contents$, NoPutReal#, -1, -1,-1)
      END IF
      INCR I%
    LOOP UNTIL FNIn%(%OverWritten , CellStatus%)<>%True
    CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
    CellStatus% = %Constant
    IF IsForm% THEN
      CALL AddSet(%Formula ,CellStatus%)
    END IF
    Value# = EvalResult#
    CALL PutRec(Fx%, Fy%, CellStatus%, CHR$(0),Value#,-1,-1,-1)
  END IF
END SUB

SUB GetCell(Fx%, Fy%)
' procedure GetCell gets the contents of a cell from the user.
' This routine gets all input entered by the user. Procedure
' GetCell then initializes the temporary variable "S" with the last
' read character. Depending on this character, it then calls
' GetFormula, GetText, or EditCell.

  LOCAL S$, ErrorPosition%, NewStatus%, EvalResult#,I%,Abort%
  SHARED Ch$,Autocalc%,NoPutReal#,EditCellMode%,GlobFx%,GlobFy%

  S$ = Ch$
  ErrorPosition% = 0
  Abort% = %FALSE
  NewStatus% = 0
  EvalResult# = NoPutReal#
  Isform%=%False
  EditCellMode% = %True

  IF FNInCharSet%(Ch$,"0123456789+-.()") THEN
    NewStatus% = %Constant
    CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
    IF FN In%(%Formula , CellStatus%)<>%True  THEN
      CALL ClearStat
      CALL ClearCells(Fx%, Fy%)
      CALL GetFormula(FX%,FY%, IsForm%, ErrorPosition%, S$, EvalResult#, _
		      NewStatus%)
    ELSE
      CALL Flash(13, "Edit formula Y/N?", %TRUE )
      DO
        CALL ReadKBD(Char$)
      LOOP UNTIL FNInCharSet%(ucase$(Char$),"YN")
      CALL Flash(13, "                 ", %FALSE )
      IF ucase$(Char$) = "Y" THEN
        CALL EditCell(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, _
		      NewStatus%)
      ELSE
        Abort% = %TRUE
      END IF
    END IF
  ELSE
    CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
    IF Ch$ = CHR$(%EditKey ) THEN
      CALL LeaveCell(Fx%,Fy%)
      NewStatus% = 0
      IF FNin%(%Txt ,CellStatus%) THEN CALL AddSet(%Txt ,NewStatus%)
      IF FNin%(%Constant ,CellStatus%) THEN CALL AddSet(%Constant ,NewStatus%)
      CALL EditCell(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, _
      		    NewStatus%)
    ELSE
      IF FN In%(%Formula , CellStatus%) THEN
        CALL Flash(13, "Edit formula Y/N?", %TRUE )
        DO
          CALL ReadKBD(Char$)
        LOOP UNTIL FNInCharSet%(ucase$(Char$),"YN")
        CALL Flash(13, "                 ", %FALSE )
        IF ucase$(Char$) = "Y" THEN
          CALL EditCell(FX%,FY%,IsForm%, ErrorPosition%, S$, EvalResult#, _
          		NewStatus%)
        ELSE
          Abort% = %TRUE
        END IF
      ELSE
        NewStatus% = %Txt
        CALL ClearCells(Fx%, Fy%)
        CALL GetText(Fx%, Fy%, ErrorPosition% , S$ )
      END IF
    END IF
  END IF
  IF Abort%=%False  THEN  ' DO necessary updating
    IF S$ <> CHR$(&HFF) THEN
      CALL UpDateCells(Fx%, Fy%, IsForm%, ErrorPosition%, S$, EvalResult#, _
      		       NewStatus%)
    END IF
    CALL GotoCell(Fx%, Fy%)
    CALL GetRec(Fx%, Fy%, CellStatus%, Contents$, Value#, Dec%, Fw%,CellColor%)
    IF AutoCalc%=%True AND ( FNIn%(%Constant , CellStatus%) ) THEN
       CALL Recalculate
    END IF
    IF FN In%(%Txt , NewStatus%) THEN
      LOCATE Fy% + 1, 3
      CALL ClrEol
      FOR I% = %FxMax  to %FxMin  step -1
        CALL LeaveCell(I%, Fy%)
      NEXT I%
    END IF
  END IF
  CALL Flash(13, "                  ", %FALSE )
  SELECT CASE CHR$(EditCellMode%)
    CASE CHR$(5)
      CALL MoveUp
    CASE CHR$(24)
      CALL MoveDown
    CASE CHR$(4)
      CALL MoveRight
    CASE CHR$(19)
      CALL MoveLeft
    CASE ELSE
      CALL GotoCell(Fx%, Fy%)
 END SELECT
 EditCellMode%=%False

END SUB ' END procedure GetCell

SUB Format
  ' procedure Format is used to modify the numeric format of a range of cells
  ' in the current column

  LOCAL J%, FromLine%, ToLine%, Lock%, S$, D%, F%
  SHARED Globfx%,Globfy%,Getint%,NoPutReal#

  GetInt%=%True
  CALL NormVideo
  CALL Msg("Column width (if larger than 11, next column will be locked) : ")
  CALL GetLine(pos(0), csrlin, 2, %True , 0, S$)
  IF S$<>CHR$(255) THEN
     Fw%=VAL(S$)
     IF Fw%<%FieldWidth  THEN Fw%=%FieldWidth  ELSE_
        IF Fw%>22 AND GlobFx%<%FxMax  THEN Fw%=22 ELSE_
           IF Fw%>11 AND GlobFx%>=%FxMax  THEN Fw%=11
  END IF
  CALL Msg("Number of Decimal (Max 11) enter -1 for scientific notation : ")
  CALL GetLine(pos(0), csrlin, 2, %True , 0, S$)
  IF S$<>CHR$(255) THEN
     Dec%=VAL(S$)
     IF Dec%<-1 THEN Dec%=-1 ELSE IF Dec%>11 THEN Dec%=11
     IF Dec%+1 >= Fw% THEN Dec% = Dec% -1    ' handle # dec places = width
  END IF
  CALL Msg("From which line in column " + CHR$(GlobFx%) + " : ")
  CALL GetLine(pos(0), csrlin, 2, %True , 0, S$)
  IF S$<>CHR$(255) THEN
    FromLine%=VAL(S$)
    IF FromLine%<%FyMin  THEN FromLine%=%FyMin  ELSE_
       IF FromLine%>%FyMax  THEN FromLine%=%FyMax
    CALL Msg("To which line in column " + CHR$(GlobFx%) + " : ")
    CALL GetLine(pos(0), csrlin, 2, %True , 0, S$)
    IF S$<>CHR$(255) THEN
      ToLine%=VAL(S$)
      IF ToLine%<%FyMin  THEN
        ToLine%=%FyMin
      ELSEIF ToLine% > %FyMax  THEN
        ToLine%=%FyMax
      END IF
      IF FromLine%>Toline% THEN SWAP FromLine%,Toline%
      IF Fw% > 11 THEN
        Lock% = %TRUE
      ELSE
        Lock% = %FALSE
      END IF
      FOR J% = FromLine% to ToLine%
        CALL PutRec(GlobFx%, J%, -1, CHR$(0), NoPutReal#, Dec%, Fw%,-1)
        IF GlobFx%<%FxMax  THEN
          CALL GetRec(GlobFx%+1, J%, CellStatus%, Contents$, Value#, _
          		 D%,F%,CellColor%)
          IF Lock% THEN
            CALL AddSet(%Locked ,CellStatus%)
            CALL AddSet(%Txt ,CellStatus%)
            Contents$=""
            CALL PutRec(GlobFx%+1, J%, CellStatus%, Contents$, NoPutReal#, _
            		   D%, F%,-1)
          ELSE
            CALL SubSet(%Locked ,CellStatus%)
            CALL PutRec(GlobFx%+1, J%, CellStatus%, CHR$(0), NoPutReal#, _
            		   Dec%, Fw%,-1)
          END IF
        END IF
      NEXT J%
      CALL Update
    END IF
  END IF

  CALL GotoCell(GlobFx%,GlobFy%)
  GetInt%=%False

END SUB ' END procedure Format

SUB GetCellColor

  LOCAL S$,C1%,C2%
  SHARED GlobFx%,GlobFy%,Getint%,NoPutReal#,Enter$,ColorHelp%

  Getint%=%True : ColorHelp%=%False
  CALL GetRec(GlobFx%, GlobFy%, CellStatus%, Contents$, Value#, Dec%, Fw%, CellColor%)
  DO
    CALL Msg("Enter foreground color number (1 to 31) or "+Enter$+_
             " for Help : ")
    CALL  GetLine(pos(0),csrlin,2,%True ,0,S$)
    IF s$="" AND ColorHelp%=%false THEN CALL ColorHelp
  LOOP UNTIL s$<>""
  IF S$<>CHR$(255) THEN
    C1%=VAL(S$)
    IF C1%<0 or C1%>31 THEN C1%=CellColor% \ 256
    DO
      CALL Msg("Enter background color number (0 to 7) or "+Enter$+_
               " for Help : ")
      CALL  GetLine(pos(0),csrlin,2,%True ,0,S$)
      IF s$="" AND ColorHelp%=%false THEN CALL ColorHelp
    LOOP UNTIL s$<>""
    IF S$<>CHR$(255) THEN
      C2%=VAL(S$)
      IF C2%<0 or C2%>7 THEN C2%=CellColor% mod 256
      IF (C1%<>0 or C2%<>0) AND (C1%<>0 or C2%<>7) THEN
         CellColor%=C1%*256+C2%
      END IF
      CALL PutRec(GlobFx%, GlobFy%, -1, CHR$(0), NoPutReal#, -1, -1, _
		  CellColor%)
      CALL LeaveCell(GlobFx%,GlobFy%)
    END IF
  END IF
  Getint%=0
  IF ColorHelp%=%True THEN CALL update
END SUB

SUB ColorHelp

  LOCAL i%,j%
  SHARED ColorHelp%

  ColorHelp%=%True
  COLOR 10,0
  LOCATE 4,4
  PRINT "Ú"+STRING$(71,"Ä")+"¿"
  COLOR 0,10 : LOCATE 4,33 : PRINT "  Color patterns  " : COLOR 10,0
  FOR i%=1 to 16
    LOCATE ,4  : PRINT "³"+SPACE$(71);"³"
  NEXT
  LOCATE ,4 : PRINT "À"+STRING$(71,"Ä")+"Ù"
  COLOR 15,0 : LOCATE 6,7
  PRINT "                       1                   2                   3  "
  LOCATE ,7
  PRINT "   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1"
  LOCATE ,7 : COLOR 7,0
  PRINT " Ú"+STRING$(65,"Ä")+"¿"
  FOR i%=0 to 7
    LOCATE ,7
    COLOR 15,0 : PRINT using "#";i%;
    COLOR 7,0 : PRINT "³";
    FOR j%=0 to 31
      COLOR j%,i% : PRINT " ";CHR$(4);
    NEXT
    PRINT " ";
    COLOR 7,0 : PRINT "³"
  NEXT
  LOCATE ,7 : PRINT " À"+STRING$(65,"Ä")+"Ù"
  COLOR 10,0 : PRINT
  LOCATE ,6 : PRINT " Numbers 0-31 FOR foreground color, ";
  PRINT "Numbers 0-7 for background color"
  CALL NormVideo
END SUB

SUB Commands
' procedure Commands is called from the programs main loop when the user
' types "/" The procedure in turn calls the appropriate procedure based
' on the user's response to the menu displayed.

  SHARED GLOBFX%,GLOBFY%,CalcExit%,Border%,FileName$,BeginTimer

  LOCATE 24, 1
  COLOR %HighLightColor,0 : PRINT "A"; : COLOR %NormColor,0 : PRINT "uto,";
  COLOR %HighLightColor,0 : PRINT "B"; : COLOR %NormColor,0 : PRINT "order,";
  COLOR %HighLightColor,0 : PRINT "C"; : COLOR %NormColor,0 : PRINT "olor,";
  COLOR %HighLightColor,0 : PRINT "D"; : COLOR %NormColor,0 : PRINT "os,";
  COLOR %HighLightColor,0 : PRINT "F"; : COLOR %NormColor,0 : PRINT "ormat,";
  COLOR %HighLightColor,0 : PRINT "G"; : COLOR %NormColor,0 : PRINT "oto,";
  COLOR %HighLightColor,0 : PRINT "H"; : COLOR %NormColor,0 : PRINT "elp,";
  COLOR %HighLightColor,0 : PRINT "I"; : COLOR %NormColor,0 : PRINT "nit,";
  COLOR %HighLightColor,0 : PRINT "L"; : COLOR %NormColor,0 : PRINT "oad,";
  COLOR %HighLightColor,0 : PRINT "P"; : COLOR %NormColor,0 : PRINT "rint,";
  COLOR %HighLightColor,0 : PRINT "Q"; : COLOR %NormColor,0 : PRINT "uit,";
  COLOR %HighLightColor,0 : PRINT "R"; : COLOR %NormColor,0 : PRINT "ecalc,";
  COLOR %HighLightColor,0 : PRINT "S"; : COLOR %NormColor,0 : PRINT "ave,";
  COLOR %HighLightColor,0 : PRINT "U"; : COLOR %NormColor,0 : PRINT "pdate";
  PRINT "?";
  CALL ReadKBD(Char$)
  Char$ = ucase$(Char$)
  SELECT CASE Char$     '
    CASE "Q"
      CalcExit%=%True   ' EXIT from the calc
    CASE "F"
      CALL Format       ' format a range of cells
    CASE "S"
      CALL save         ' save the current spreadsheet to a file
    CASE "L"
      FileName$=""
      CALL load         ' load a spreadsheet from a file
    CASE "H"
      CALL Help         ' CALL the help procedure
      CALL Update
    CASE "R"
      CALL Recalculate  ' recalculate the spreadsheet
    CASE "A"
      CALL Auto         ' toggle AutoCalc ON/OFF
    CASE "U"
      CALL Update       ' redraw the screen
    CASE "I"
      CALL ClearSheet   ' clear spreadsheet
    CASE "P"
      CALL PrintSheet   ' PRINT spreadsheet to file or printer
    CASE "B"            ' Border on/off
      Border%=NOT Border%
      CALL Update
    CASE "D"            ' Dos access
      CALL DosShell
      CALL Update
    CASE "G"
      CALL  MoveToCell
    CASE "C"
      CALL  GetCellColor
    CASE ELSE
      Char$=""          ' No more command available
  END SELECT
  IF CalcExit% THEN
     CLS
  ELSE
     CALL Grid
     CALL GotoCell(GlobfX%, GlobFY%)
  END IF

END SUB
