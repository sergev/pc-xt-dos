'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC2.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: Init and display and clear spreadsheet grid		     ³
'³									     ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


SUB Grid
' Grid displays the entire spreadsheet grid AND command line

  SHARED XPos%(),BORDER%
  LOCAL I%

  IF Border%=%True  THEN
    LOCATE 1,1 : COLOR %HighLightColor,9
    FOR I% = %FXMin  TO %FXMax
      PRINT tab(Xpos%(I%));"³    ";CHR$(I%);"    ";
    NEXT I%
    PRINT "³";
    LOCATE 2,1
    FOR I% = %FYMin  TO %FYMax
      PRINT using "##";I%
    NEXT I%
    CALL DISPAUTO
    CALL Flash(31,"Type / for Commands, F2 for Edit", %FALSE  )
    COLOR %HighLightColor,0
    LOCATE,36 : PRINT "/";
    LOCATE,52 : PRINT "F2";
    CALL NormVideo
  END IF
END SUB

SUB DISPAUTO
' Display the AutoCalc state
  SHARED AutoCalC%,BORDER%

  IF BORDER%=%True  THEN
    CALL Flash( 66,"AutoCalc is", %FALSE  )
    LOCATE ,78 : COLOR %HighLightColor,0
    IF AutoCalc%=%True  THEN
      PRINT "ON ";
    ELSE
      PRINT "OFF";
    END IF
    CALL NormVideo
 END IF
END SUB

SUB Init
' Init initializes most of the needed global variables AND arrays.

  SHARED  GlobFX%, GlobFY%, AutoCalc%, Border%
  LOCAL I%,J%

    FOR I% = %FXMin  TO %FXMax       ' Set up each cell record
      FOR J% = %FYMin  TO %FYMax     '
        CALL PutRec( I%,J%,%Txt ,"",0.0,%NumDecPlaces ,%FieldWidth , 7*256+0)
      NEXT J%
    NEXT I%
    Border%   = %True
    AutoCalc% = %TRUE
    GlobFX%   = %FXMin                 ' We will start with autocalc on
    GlobFY%   = %FYMin                 ' Start at the upper left corner (A1)

END SUB

SUB ClearSheet
' ClearSheet clears the all the spreadsheet cells and redraws the spreadsheet.

  LOCAL Ch$

  CALL NormVideo
  CALL ClearStat
  LOCATE %StatusLine ,1            ' Make sure you go to the prompt location
                                   ' after a ClearStat, because ClearStat does
                                   ' not restore the previous cursor position
  PRINT "Clear this worksheet (Y/N) ? "; ' Make sure the user wants to do this
  DO
    CALL Readkbd(CH$)
  LOOP UNTIL UCASE$(Ch$)="Y" OR UCASE$(Ch$)="N"
  PRINT UCASE$(Ch$);
  IF UCASE$(Ch$)="Y" THEN
    CALL Init                      'initialize the cells of the spreadsheet
    CLS
    CALL Grid                      ' Redraw the grid
  END IF

END SUB

