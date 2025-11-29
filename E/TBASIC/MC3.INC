'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC3.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: Display Cells and move around the spreadsheet                ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

 SUB DisplayType(FX%,FY%)
 ' Display the cell's contents and type

   LOCAL CellStatus%, FW%, Contents$, Value#, Dec%, CellColor%
   SHARED Border%

   CALL GetRec( FX%,FY%,CellStatus%,Contents$,Value#,Dec%,FW%, CellColor% )
   CALL ClearStat
   IF Border%=%True  THEN
    LOCATE %FlashLine ,1
    PRINT "[";CHR$(FX%);MID$(STR$(FY%),2);",";
    IF FNIN%( %Formula ,CellStatus% ) THEN
      PRINT "Formula]  ";
    ELSEIF FNIN%(%Constant ,CellStatus% ) THEN
      PRINT "Numeric]  ";
    ELSEIF FNIN%(%Txt ,CellStatus% ) THEN
      PRINT "Text]     ";
    ELSE
      PRINT "Indefined]";
    END IF
    LOCATE %StatusLine ,1
    COLOR 9,0
    PRINT Contents$;
    CALL NormVideo
  END IF
END SUB

SUB GotoCell(FX%,FY%)
' goto a cell and display it

    LOCAL CellStatus%, FW%,Contents$, Value#, Dec%, L%
    SHARED XPOS%(),Border%

    CALL GetRec( FX%,FY%,CellStatus%,Contents$,Value#,Dec%,FW%,CellColor% )
    IF Border%=%True  THEN
       LOCATE 1,XPos%(FX%),0 : COLOR 9,11
       PRINT "³    ";CHR$(FX%);"     ³";
       LOCATE FY%+1,1 : PRINT USING "##";FY%;
    END IF
    CALL InvVideo
    LOCATE FY%+1,XPos%(FX%),1
    IF FNIN%( %Txt ,CellStatus% ) THEN
      IF LEN(Contents$)>%CellWidth  THEN L%= LEN(Contents$) ELSE L%=%CellWidth
      PRINT LEFT$(Contents$+SPACE$(L%),L%);
    ELSE
      PRINT USING FNMASK$(FW%,DEC%);Value#;
    END IF
    CALL NormVideo
    CALL DisplayType(FX%,FY%)          ' Show the cell type on the screen
    LOCATE FY%+1,XPos%(FX%),1          ' Go to the location

END SUB

SUB LeaveCell(FX%,FY%)
' Leave a cell

  LOCAL CellStatus%, FW%,Contents$, Value#, Dec%,L%
  SHARED Xpos%(),Border%

    CALL GetRec( FX%,FY%,CellStatus%,Contents$,Value#,Dec%,FW%, CellColor% )
    IF Border%=%True  THEN
       LOCATE 1,XPos%(FX%),0 : COLOR 15,9
       PRINT "³    ";CHR$(FX%);"     ³";
       LOCATE FY%+1,1 : PRINT USING "##";FY%;
    END IF
    COLOR CellColor% \ 256, CellColor% MOD 256
    LOCATE FY%+1,XPos%(FX%),1
    IF FNIN%( %Txt ,CellStatus% ) THEN
      IF LEN(Contents$)>%CellWidth  THEN L%= LEN(Contents$) ELSE L%=%CellWidth
      PRINT LEFT$(Contents$+SPACE$(L%),L%);
    ELSE
      PRINT USING FNMASK$(FW%,DEC%);Value#;
    END IF
    CALL LowVideo
END SUB

SUB UpDate
' Update restores the screen with all the current values of the cells.

  LOCAL FX%, FY%, CellStatus%, FW%, Contents$, Value#, Dec%
  SHARED GlobFX%,GlobFY%

    CLS
    CALL Grid
    FOR FX% = %FXMin  TO %FXMax
      FOR FY% = %FYMin  TO %FYMax
        CALL GetRec( FX%,FY%,CellStatus%,Contents$,Value#,Dec%,FW%, CellColor%)
        IF Contents$ <> "" OR FNIn%(CellStatus%,%Constant +%Calculated ) THEN
          CALL LeaveCell( FX%,FY% )
        END IF
      NEXT FY%
    NEXT FX%
    CALL GotoCell( GlobFX%, GlobFY% )

END SUB

SUB MoveHome
'Move to cell A1

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%, Contents$, Value#, Dec%

  CALL LeaveCell( GlobFX%, GlobFY% )
  CALL GetRec(%FxMin,%FyMin,CellStatus%,Contents$,Value#,Dec%,FW%,CellColor%)
  IF (CellStatus% AND ( %OverWritten  + %Locked  )) = 0  THEN
    GlobFx%=%FxMin
    GlobFy%=%FyMin
    CALL GotoCell( GlobFX%, GlobFY% )
  END IF
END SUB

SUB MoveEnd
'Move to cell G21

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%, Contents$, Value#, Dec%

  CALL GetRec(%FxMax,%FyMax,CellStatus%,Contents$,Value#,Dec%,FW%,CellColor%)
  IF (CellStatus% AND ( %OverWritten  + %Locked  )) = 0  THEN
    CALL LeaveCell( GlobFX%, GlobFY% )
    GlobFx%=%FxMax
    GlobFy%=%FyMax
    CALL GotoCell( GlobFX%, GlobFY% )
  END IF
END SUB

SUB MoveDown
' Move down the spreadsheet

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%, Contents$, Value#, Dec%, Start%

    CALL LeaveCell( GlobFX%, GlobFY% )
    Start% = GlobFY%
    DO
      GlobFY% = GlobFY% + 1
      IF GlobFY% > %FYMax  THEN GlobFY% = %FYMin
      CALL GetRec( GlobFX%,GlobFY%,CellStatus%,Contents$,Value#,Dec%,FW%,CellColor% )
    LOOP UNTIL ( (CellStatus% AND ( %OverWritten  + %Locked  )) = 0 )_
               OR ( GlobFY% = Start% )
    IF GlobFY% <> Start% THEN CALL GotoCell( GlobFX%, GlobFY% )
END SUB

SUB MoveUp
' Move up the spreadsheet

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%,Contents$, Value#, Dec%, Start%

    CALL LeaveCell( GlobFX%, GlobFY% )
    Start% = GlobFY%
    DO
      GlobFY% = GlobFY% - 1
      IF GlobFY% < %FYMin  THEN GlobFY% = %FYMax
      CALL GetRec( GlobFX%,GlobFY%,CellStatus%,Contents$,Value#,_
                   Dec%,FW%,CellColor% )
    LOOP UNTIL ( (CellStatus% AND ( %OverWritten +%Locked  )) = 0 ) OR _
               ( GlobFY% = Start% )
    IF GlobFY% <> Start% THEN CALL GotoCell( GlobFX%, GlobFY% )
END SUB

SUB MoveRight
' Move to the right column in the spreadsheet

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%, Contents$, Value#, Dec%, Start%

    CALL LeaveCell( GlobFX%, GlobFY% )
    Start% = GlobFX%
    DO
      GlobFX% = GlobFX% + 1
      IF GlobFX% > %FXMax  THEN
        GlobFX% = %FXMin
        GlobFY% = GlobFY% + 1
        IF ( GlobFY% > %FYMax  ) THEN GlobFY% = %FYMin
      END IF
      CALL GetRec( GlobFX%,GlobFY%,CellStatus%,Contents$,Value#,Dec%,_
                   FW%,CellColor% )
    LOOP UNTIL ( (CellStatus% AND ( %OverWritten +%Locked  )) = 0 ) OR _
               ( GlobFX%=Start% )
    IF (GlobFX%<>Start%) THEN CALL GotoCell( GlobFX%, GlobFY% )

 END SUB

SUB MoveLeft
' Move to the left column in the spreadsheet

  SHARED GlobFX%,GlobFY%
  LOCAL CellStatus%, FW%, Contents$, Value#, Dec%, Start%

    CALL LeaveCell( GlobFX%, GlobFY% )
    Start% = GlobFX%
    DO
     GlobFX% = GlobFX% - 1
      IF GlobFX% < %FXMin  THEN
        GlobFX% = %FXMax
        GlobFY% = GlobFY% - 1
        IF GlobFY% < %FYMin  THEN GlobFY% = %FYMax
      END IF
      CALL GetRec( GlobFX%,GlobFY%,CellStatus%,Contents$,Value#,Dec%,_
                   FW%,CellColor% )
    LOOP UNTIL ( (CellStatus% AND ( %OverWritten +%Locked  )) = 0 ) OR _
               ( GlobFY% = Start% )
    IF GlobFX% <> Start% THEN CALL GotoCell( GlobFX%,GlobFY% )
END SUB

SUB MoveToCell
' move to a specified cell

    SHARED GlobFx%,GlobFy%
    LOCAL S$

    CALL Msg("Enter the cell reference where you want to go : ")
    CALL GetLine(POS(0),CSRLIN,3,%True ,0,S$)
    IF S$<>"" AND S$<>CHR$(255) THEN
       IF FNInCharSet%(LEFT$(S$,1),"ABCDEFG") AND VAL(MID$(S$,2))>=%FyMin_
          AND VAL(MID$(S$,2))<=%FyMax  THEN
          CALL LeaveCell(GlobFx%,GlobFy%)
          GlobFx%=ASC(S$)
          GlobFy%=VAL(MID$(S$,2))
       END IF
    END IF

END SUB
