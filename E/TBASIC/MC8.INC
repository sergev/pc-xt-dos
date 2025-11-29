'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC8.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: This module contains the procedures GetRec AND PutRec. These ³
'³		procedures are used to read/write records to/from the data   ³
'³		structure representing the spreadsheet.		     	     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

SUB GetRec(Col%, Row%, CellStatus%, Contents$, Value#, Dec%, Fw%, CellColor%)
  ' This procedure fetches a record from the three dimensional array
  ' data structure used to represent our spreadsheet.

  LOCAL V$, Count%
  SHARED SpreadSheet%()

    Contents$ = ""

    ' GET CellStatus% from first word of record
    CellStatus% = SpreadSheet%(Col%, Row%, %CellStatusOfs )

    ' read string out of data record into Contents$
    FOR Count%=1 to SpreadSheet%(Col%, Row%, %ContentsOfs )
      Contents$=Contents$+CHR$(SpreadSheet%(Col%,Row%,%ContentsOfs +Count%))
    NEXT Count%

    ' move the contents of the Value field into Value# using CVD
    V$=""
    FOR Count%=0 to 3
       V$=V$+mki$(SpreadSheet%(Col%,Row%,%ValueOfs +Count%))
    NEXT Count%
    Value#=CVD(V$)

    ' assign the value of Dec%
    Dec%=SpreadSheet%(Col%, Row%, %DecOfs )

    ' assign the value of Fw%
    Fw%=SpreadSheet%(Col%, Row%, %FwOfs )

    ' assign the value of CellColor%
    CellColor%=SpreadSheet%(Col%, Row%, %ColorOfs )

END SUB ' END the procedure GetRec

SUB PutRec(Col%, Row%, CellStatus%, Contents$, Value#, Dec%, Fw%, CellColor%)
  ' This procedure writes a record to the three dimensional array
  ' data structure used to represent our spreadsheet. Note that IF
  ' any of the numeric parameters are = -1 or the string parameter
  ' has a null character in the first position, the field is
  ' undefined and the corresponding field in the record will NOT be
  ' modified.

  LOCAL Count%,V$
  SHARED SpreadSheet%(),NoPutReal#

    ' write CellStatus% to the record
    IF CellStatus% <> -1 THEN  ' field is defined
      SpreadSheet%(Col%, Row%, %CellStatusOfs ) = CellStatus%
    END IF ' END IF THEN block

    ' write the string Contents$ to the record
    IF LEFT$(Contents$,1) <> CHR$(0) THEN ' field is defined
      ' assign the length byte
      SpreadSheet%(Col%, Row%, %ContentsOfs ) = LEN(Contents$)
      ' copy each of the characters into a word of the record
      FOR Count% = 1 to SpreadSheet%(Col%, Row%, %ContentsOfs )
        ' GET the NEXT character
        SpreadSheet%(Col%,Row%,%ContentsOfs +Count%)=_
                                               ASC(MID$(Contents$,Count%,1))
      NEXT Count%
    END IF ' END IF THEN block

    ' using MKD$ to write Value# to the record
    IF Value# <> NoPutReal# THEN ' is defined
       V$=mkd$(Value#)
       FOR Count%=0 to 3
         SpreadSheet%(Col%,Row%,%ValueOfs +Count%)=CVI(MID$(V$,Count%*2+1,2))
       NEXT
    END IF

    IF Fw% <> -1 THEN ' is defined
      SpreadSheet%(Col%, Row%, %FwOfs ) =  Fw%
      SpreadSheet%(Col%, Row%, %DecOfs ) = Dec%
    END IF ' END IF THEN block

    IF CellColor%<>-1 THEN
      SpreadSheet%(Col%, Row%, %ColorOfs )=CellColor%
    END IF

END SUB ' end the procedure PutRec


