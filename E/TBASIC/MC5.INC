'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC5.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: This module contains the procedures to evaluate formulas in  ³
'³		the spreadsheet and in general recalculate the entire	     ³
'³		spreadsheet.						     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

SUB NextChar
' this procedure returns the next character in the formula of the cell
' currently being evaluated

  SHARED Eofline$,Position%,FormulaStr$,NextChar$

  DO
    INCR Position%
    IF Position% <= LEN(FormulaStr$) THEN
      NextChar$ = MID$(FormulaStr$, Position%, 1)
    ELSE
      NextChar$ = EofLine$
    END IF
  LOOP UNTIL NextChar$<>" "
END SUB


DEF FN Fact#(R#)
' recursive Factorial of R#

  IF (R#>0.0) AND (R#<34.0) THEN
    FNFact#=R#*FNFact#(R#-1)
  ELSE
    FNFact#=1.0
  END IF

END DEF


DEF FNFactor#
' function Factor is the meat of the procedure Evaluate. Within this the
' procedure the current expression is actually evaluated. Using nested
' if-then-else statements, the function determines if the sub-expression
' is a number, the sum of a sub-range of cells (i.e. A1>A5), or a function
' (i.e. ABS(x) )

  LOCAL E%,EE%,L%,Sf% ' temporary variables
  LOCAL Found%        ' boolean flag - standard function Found or not
  LOCAL F#            ' F holds value returned by recursive calls to Factor
  LOCAL CellSum#      ' Sum of a cell range
  LOCAL Sf$           ' standard function string variable
  LOCAL ExpFX%        ' ExpEFX and ExpFY hold the positions of the cells
  LOCAL ExpFY%        ' referenced in any formulas
  LOCAL Start%,Exy$           ' temporary cell references
  LOCAL OldExpFX%,OldExpFy%   '
  LOCAL CellStatus%,Contents$         ' Cell attribute variables
  LOCAL Value#, Dec%, Fw%,CellColor%  '

  F# = NoPutReal#
  IF FNInCharSet%(NextChar$, Numbers$) THEN
    Start% = Position%
    DO
      CALL NextChar
    LOOP UNTIL FNInCharSet%(NextChar$, Numbers$)<>%True
    IF NextChar$ = "." THEN    ' is decimal point
      DO
         CALL NextChar
      LOOP UNTIL FNInCharSet%(NextChar$, Numbers$)<>%True
    END IF
    IF NextChar$ = "E" THEN
      CALL NextChar
      DO
        CALL NextChar
      LOOP UNTIL FNInCharSet%(NextChar$, Numbers$)<>%True
    END IF
    ' now get the value of the number
    F# = VAL(MID$(FormulaStr$, Start%, Position%-Start%))
  ELSEIF NextChar$ = "(" THEN
    'Parenthesis expression
    CALL NextChar
    IF NextChar$ = "+" THEN CALL NextChar
    F# = FNExpression#
    IF NextChar$ = ")" THEN
      CALL NextChar
    ELSE
      ErrorPosition% = Position%
    END IF
  ELSEIF FNInCharSet%(NextChar$, "ABCDEFG") AND _
         FNInCharSet%(MID$(FormulaStr$+" ",Position%+1,1),Numbers$+" ") THEN
    ' Cell reference expression
    ExpFX%=ASC(NextChar$)
    CALL NextChar
    IF FN InCharSet%(NextChar$, Numbers$) THEN
      F# = 0
      Exy$ = NextChar$
      CALL NextChar
      IF FN InCharSet%(NextChar$, Numbers$) THEN
        Exy$ = Exy$ + NextChar$
        CALL NextChar
      END IF
      ' GET Cell Number
      ExpFy% = VAL(Exy$)
      IF ExpFy%>%FyMax  THEN ExpFy%=%FyMax
      IsFormula% = %TRUE
      ' now check if the content of the cell referenced in the formula
      ' is a constant. If so then verify that it has been calculated.
      ' If the constant has been calculated then make a recursive call
      ' to the procedure Evaluate to evaluate the contents of the cell.
       CALL GetRec(ExpFx%, ExpFy%, CellStatus%, Contents$, Value#, _
                  Dec%, Fw%,CellColor%)
      IF FNIn%( %Constant , CellStatus% ) AND _
              ( FNIn%( %Calculated , CellStatus% )<>%True )  THEN
        CALL Evaluate(Form%, Contents$, F#, ErrorPosition%)
        IsFormula% = %TRUE
        CALL GetRec(ExpFx%, ExpFy%, CellStatus%, Contents$, Value#, _
                    Dec%, Fw%,CellColor%)
        CALL AddSet(%Calculated ,CellStatus%)
        CALL PutRec(ExpFx%, ExpFy%, CellStatus%, CHR$(0), NoPutReal#, _
                    -1, -1, -1)
      ELSE
        IF  FNIn%( %Txt , CellStatus%)<>%True  THEN F# = Value#
      END IF
      IF NextChar$ = ">" THEN
      ' it's a cell range operator
        OldExpFX% = ExpFX%
        OldExpFY% = ExpFY%
        CALL NextChar
        ExpFx% = ASC(NextChar$)
        CALL NextChar
        IF FNInCharSet%(NextChar$, Numbers$) THEN
          Exy$ = NextChar$
          CALL NextChar
          IF FNInCharSet%(NextChar$, Numbers$) THEN
            Exy$ = Exy$ + NextChar$
            CALL NextChar
          END IF
          ' now get the Cell number
          ExpFy% = VAL(Exy$)
          IF ExpFy%>%FyMax  THEN ExpFy%=%FyMax
           CellSum# = 0.0
           ' visit each cell specified in SUB-range of formula
          FOR Momo% = OldExpFy% to ExpFy%
            FOR Ida% = OldExpFx% to ExpFx%
              F# = 0.0
              CALL GetRec(Ida%, Momo%, CellStatus%, Contents$, Value#, _
                          Dec%, Fw%,CellColor%)
              IF FN In%( %Constant , CellStatus% ) AND _
                (FNIn%( %Calculated , CellStatus%)<>%True ) THEN
                CALL Evaluate(Form%, Contents$, F#, ErrorPosition%)
                ' update CellStatus to indicate that the cells' value has
                ' been calculated
                CALL GetRec(Ida%, Momo%, CellStatus%, Contents$, _
                            Value#, Dec%, Fw%,CellColor%)
                CALL AddSet(%Calculated ,CellStatus%)
                CALL PutRec(Ida%, Momo%, CellStatus%, CHR$(0), _
                            NoPutReal#, -1, -1, -1)
              ELSE
                IF NOT FNIn%( %Txt , CellStatus% ) THEN F# = Value#
              END IF
              CellSum# = CellSum# + F#
            NEXT
          NEXT
          F# = CellSum#
        END IF
      END IF
    END IF
  ELSE
   ' Standard function
   Found% = %FALSE
   FOR Sf% = %Fabs  to %Ffact
     ' step through all possible Standard functions
     IF Found%<>%True  THEN
       L% = LEN(StandardFunction$(Sf%))
       IF MID$(FormulaStr$, Position%, L%) = _
                         StandardFunction$(Sf%) THEN
         Position% = Position% + L% - 1
         CALL NextChar
         F# = FNFactor#
         SELECT CASE Sf%
           CASE %Fabs
             F# = ABS(F#)
           CASE %Fsqrt
             IF F# > 0 THEN F# = SQR(F#) ELSE F# = -1
           CASE %Fsqr
             F# = F#^2
           CASE %Fsin
             F# = SIN(F#)
           CASE %Fcos
             F# = COS(F#)
           CASE %Farctan
             F# = ATN(F#)
           CASE %Fln
             F# = LOG(F#)
           CASE %Flog
             F# = log10(F#)
           CASE %Fexp
             F# = EXP(F#)
           CASE %Fint
             F# = INT(F#)
           CASE %Fsgn
             F# = SGN(F#)
           CASE %Frnd
             F# = RND(F#)
           CASE %Ffact
             F# = FNFact#(F#)
         END SELECT
         Found% = %TRUE
       END IF
     END IF
   NEXT Sf%
   IF Found%<>%True  THEN ErrorPosition% = Position%
  END IF
  FNFactor# = F#
END DEF

DEF FNSignedFactor#
' this function first determines the sign of the expression. It then
' calls the procedure factor to get the value of the expression.

  IF NextChar$ = "-" THEN
    CALL NextChar
    FN SignedFactor# = -FNFactor#
  ELSE
    FNSignedFactor# = FNFactor#
  END IF
END DEF

DEF FN Term#

 LOCAL T#

  T# = FNSignedFactor#
  WHILE NextChar$ = "^"
    CALL NextChar
    T# = T#^FNSignedFactor#
  WEND
  FNTerm# = T#
END DEF

DEF FN SimpleExpression#

  LOCAL SimpExp#, Opr$

  SimpExp# = FNTerm#
  WHILE FN InCharSet%(NextChar$,"*/")
    Opr$ = NextChar$
    CALL NextChar
    SELECT CASE Opr$
      CASE "*"
        SimpExp# = SimpExp# * FNTerm#
      CASE "/"
        SimpExp# = SimpExp# / FN Term#
    END SELECT
  WEND
  FN SimpleExpression# = SimpExp#
END DEF

DEF FN Expression#

  LOCAL E#, Opr$

  E# = FNSimpleExpression#
  WHILE FN InCharSet%(NextChar$, "+-")
    Opr$ = NextChar$
    CALL NextChar
    SELECT CASE Opr$
      CASE "+"
        E# = E# + FNSimpleExpression#
      CASE "-"
        E# = E# - FN SimpleExpression#
    END SELECT
  WEND
  FNExpression# = E#
END DEF

SUB Evaluate(GlobIsFormula%, F$, Value#, Er%)
' this procedure evaluates a string passed to it, the string represents
' a value or an expression or formula.

  SHARED Eofline$,Position%,FormulaStr$,NextChar$,IsFormula%,ErrorPosition%

  FormulaStr$=F$
  IF left$(FormulaStr$, 1) = "." THEN
    FormulaStr$ = "0" + FormulaStr$
  ELSEIF left$(FormulaStr$, 1) = "+" THEN
    CALL Delete(FormulaStr$, 1, 1)
  END IF
  IsFormula% = %FALSE
  ErrorPosition% = %FALSE
  Position% = 0
  CALL NextChar
  Value# = FNExpression#
  GlobIsFormula%=IsFormula%
  IF NextChar$ = EofLine$ THEN
    ErrorPosition% = 0
  ELSE
    Errorposition% = Position%
  END IF
  Er%=ErrorPosition%
END SUB

SUB Recalculate
' this procedure steps through the entire spreadsheet recalculating each cell

  LOCAL  Rfx%, Rfy%, OldValue#, Er%, CellStatus%, Contents$, Value#
  LOCAL  Dec%, Fw%, CellColor%, form%
  SHARED Globfx%,Globfy%,Xpos%(), NoPutReal#

  CALL ClearStat
  CALL BlinkVideo
  CALL Msg(" Computing ...")
  CALL LowVideo
  FOR Rfy% = %FyMin  to %FyMax
    FOR Rfx% = %FxMin  to %FxMax
      CALL GetRec(RFx%, RFy%, CellStatus%, Contents$, Value#, Dec%, Fw%, _
                  CellColor%)
      IF FNIn%(%Formula,CellStatus%) or FNIn%(%Constant,CellStatus%) THEN
        OldValue# = Value#
        CALL AddSet( %Calculated  , CellStatus% )
        CALL PutRec(RFx%, RFy%, CellStatus%, CHR$(0), NoPutReal#, -1, -1, -1)
        CALL Evaluate(Form%, Contents$, Value#, Er%)
        IF OldValue# <> Value# THEN
          LOCATE Rfy% + 1, Xpos%(Rfx%), 0
          color CellColor% \ 256, CellColor% mod 256
          PRINT using FNMASK$(FW%,DEC%);Value#;
        END IF
        CALL PutRec(RFx%, RFy%, CellStatus%, CHR$(0), Value#, -1, -1, -1)
      END IF
    NEXT Rfx%
  NEXT Rfy%
  CALL NormVideo
  CALL Clearstat
  CALL GotoCell( GlobFx%, GlobFy% )
END SUB

