'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC0.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: This module contains the global named constant and dim	     ³
'³              statements used in the program MicroCalc                     ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  %FALSE      = 0
  %TRUE       = 1
  NoPutReal#  = 99999.99

  %FxMin    = 65 ' lower column dimension of spreadsheet data structure
  %FxMax    = 71 ' upper column dimension of spreadsheet data structure
  %FyMin    = 1  ' lower line dimension of spreadsheet data structure
  %FyMax    = 21 ' upper line dimension of spreadsheet data structure
  %RecSize  = 79 ' size of record in spreadsheet data structure

  %CellStatusOfs  = 1   ' these constants represent the offsets in the
  %ContentsOfs    = 2   ' records in the spreadsheet, from which we must
  %ValueOfs       = 73  ' retrieve individual fields
  %DecOfs         = 77  '
  %FwOfs          = 78  '
  %ColorOfs       = 79

  %FieldWidth     = 11
  %NumDecPlaces   =  2
  %ContentsLen    = 70 ' the length of the string contents in the record

  ' declare the data structure used to represent our spreadsheet
  dim SpreadSheet%(%FxMin:%FxMax , %FyMin:%FyMax , 1:%RecSize )

  ' spreadsheet record global variables
  CellStatus% = -1
  Contents$   = chr$(0)
  Value#      = NoPutReal#
  Dec%        = -1
  Fw%         = -1

  ' cell status set
  %Constant    =  1
  %Formula     =  2
  %Txt         =  4
  %OverWritten =  8
  %Locked      = 16
  %Calculated  = 32

  %SheetSize   = 23226
  %ScreenSize  = 80
  %FlashLine   = 23
  %StatusLine  = 24

  GlobFX% = 0
  GlobFY% = 0

  Form% = %FALSE

  %CellWidth =11

  dim Xpos%(%FxMin:%FxMax ) ' declare an array to hold the column positions

    Xpos%(%FxMin )     = 3   ' initialize the array that holds the column
    Xpos%(%FXMin  + 1) = 14  ' positions of the cells
    Xpos%(%FXMin  + 2) = 25
    Xpos%(%FXMin  + 3) = 36
    Xpos%(%FXMin  + 4) = 47
    Xpos%(%FXMin  + 5) = 58
    Xpos%(%FXMin  + 6) = 69


  ' Standard Functions

    %Fabs     = 0
    %Fsqrt    = 1
    %Fsqr     = 2
    %Fsin     = 3
    %Fcos     = 4
    %Farctan  = 5
    %Fln      = 6
    %Flog     = 7
    %Fexp     = 8
    %Fint     = 9
    %Fsgn     =10
    %Frnd     =11
    %Ffact    =12

    dim StandardFunction$(%Fabs:%Ffact )

    StandardFunction$(%Fabs )    = "ABS"
    StandardFunction$(%Fsqrt )   = "SQRT"
    StandardFunction$(%Fsqr )    = "SQR"
    StandardFunction$(%Fsin )    = "SIN"
    StandardFunction$(%Fcos )    = "COS"
    StandardFunction$(%Farctan ) = "ARCTAN"
    StandardFunction$(%Fln )     = "LN"
    StandardFunction$(%Flog )    = "LOG"
    StandardFunction$(%Fexp )    = "EXP"
    StandardFunction$(%Fint )    = "INT"
    StandardFunction$(%Fsgn )    = "SGN"
    StandardFunction$(%Frnd )    = "RND"
    StandardFunction$(%Ffact )   = "FACT"

    Numbers$="0123456789"

  ' Miscellaneous

  %NormColor  = 14       ' Screen colors
  %LowColor   =  7
  %Blinkcolor = 30
  %HighLightColor = 15

  %EditKey     = 2
  EditCellMode%= %False

  Enter$      = chr$(17)+chr$(196)+chr$(217)  ' the enter key
  EofLine$    = chr$(13)                      ' end of line character
  CalcExit%   = %False                        ' To escape from the calc
  AutoCalc%   = %True
  Border%     = %True
