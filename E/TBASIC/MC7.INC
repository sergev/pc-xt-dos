'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC7.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: This module contains the routines to manipulate strings and  ³
'³		sets.							     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

SUB Insert(AddS$, S$, P%)
' This procedure inserts a string into an existing string

  S$=LEFT$(S$,P%-1 )+AddS$+MID$(S$,P%)

END SUB

SUB Delete(S$, P%, N%)
' This procedure deletes a SUB string from within a string.

  S$=LEFT$(S$,P%-1 )+MID$(S$,P%+N%)

END SUB

DEF FNInCharSet%(CheckChar$, CurSet$ )
' This function returns a result indicating IF the value in CheckChar
' exists in CurSet.

  LOCAL Exists%

  IF INSTR(CurSet$,LEFT$(CheckChar$+" ",1)) THEN
     FNInCharSet%=%True
  ELSE
     FNInCharSet%=%False
  END IF

END DEF

DEF FNSt$(V#)
' convert a value into string
  IF V#<0 THEN
    FNSt$=STR$(V#)
  ELSE
    FNSt$=MID$(STR$(V#),2)
  END IF
END DEF

DEF FNIn%( Mask%, SetNumber% )
' test if an item is included in the set
  FNIn%=-((SetNumber% AND Mask%)<>0)
END DEF

SUB AddSet(SetItem%,SetNumber%)
' Add item in the set if it doesn't exist
  IF FNin%(SetItem%,SetNumber%)=%False  THEN SetNumber%=SetNumber%+SetItem%
END SUB

SUB SubSet(SetItem%,SetNumber%)
' Subtract item in the set if it doesn't exist
  IF FNin%(SetItem%,SetNumber%)<>%False  THEN SetNumber%=SetNumber%-SetItem%
END SUB
