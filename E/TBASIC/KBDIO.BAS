'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                  KEYBOARD I/O LIBRARY EXAMPLE PROGRAM                     ³
'³									     ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³   The following program demonstrates how to use the routines      	     ³
'³   found in the Keyboard I/O Library.  The program itself serves	     ³
'³   no useful purpose other than to show how the routines can be	     ³
'³   used together to retrieve a screen of data.			     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Main Program ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    CALL InitKBDIO
    CALL DrawEditScreen
    CALL EditData
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

$INCLUDE "KBDIO.INC"  ' include library routines

SUB DrawEditScreen
' Draws input screen
LOCAL X%
  CLS
  LOCATE 6,18 : PRINT "Name.......:";
  LOCATE 7,18 : PRINT "Address....:";
  LOCATE 8,18 : PRINT "City.......:";
  LOCATE 9,18 : PRINT "State......:";
  LOCATE 10,18: PRINT "Zip........:";
  LOCATE 11,18: PRINT "Phone......:";
  LOCATE 12,18: PRINT "Birth Date.:";
  LOCATE 13,18: PRINT "Sex........:";
  LOCATE 14,18: PRINT "Balance....:";
  LOCATE 15,18: PRINT "Bill Code..:";
END SUB

SUB EditData
' Calls library routines to fetch data
LOCAL  Ptr%,TermChar$,TermSet$,FullName$,Address$,City$,State$,Zip&,_
       Phone$,BDate$,Balance#,Sex$,BCode%
SHARED ESC$, CR$, ADOWN$, AUP$

 Ptr% = 1
 TermSet$ = AUP$ + ADOWN$ + CR$ + ESC$

 'Termset defines the keys that will be able to terminate input of each
 'field.

 WHILE FNInSet(TermChar$,ESC$ + F10$) = %False
   SELECT CASE Ptr%
    ' If X position, Y position, and Length are variables, be sure
    ' to pass them as value parameters
    CASE = 1
      CALL GetAny((TermSet$),25,31,6,"#",FullName$,TermChar$,%False)
    CASE = 2
      CALL GetAny((TermSet$),25,31,7,"#",Address$,TermChar$,%False)
    CASE = 3
      CALL GetAny((TermSet$),15,31,8,"#",City$,TermChar$,%False)
    CASE = 4
      CALL GetAny((TermSet$),2,31,9,"#",State$,TermChar$,%False)
    CASE = 5
      CALL GetLongInt((TermSet$),5,31,10,Zip&,TermChar$)
    CASE = 6
      CALL GetAny((TermSet$),13,31,11,"(###)###-####",Phone$,TermChar$,%True)
    CASE = 7
      CALL GetDate((TermSet$),31,12,BDate$,TermChar$)
    CASE = 8
      CALL GetOneChar((TermSet$),31,13,Sex$,TermChar$,"MF")
    CASE = 9
      CALL GetReal((TermSet$),10,31,14,Balance#,TermChar$)
    CASE = 10
      CALL GetInteger((TermSet$),4,31,15,FamNum%,TermChar$)
    END SELECT

    'In this case AUP$ will move to the next field above the current one
    'one, ADOWN$ and CR$ will move to the next field below. ESC$ will
    'signal the end of input of all the data

    IF TermChar$ = AUP$ THEN
      IF Ptr% = 1 THEN
        Ptr% = 10
      ELSE
        Ptr% = Ptr% - 1
      END IF
    ELSEIF FnInSet(TermChar$,ADOWN$+CR$) = %True THEN
      IF Ptr% = 10 THEN
        Ptr% = 1
      ELSE
        Ptr% = Ptr% + 1
      END IF
    END IF
 WEND
END SUB


