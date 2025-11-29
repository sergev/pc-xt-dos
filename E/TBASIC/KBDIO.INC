'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³									     ³
'³			KEYBOARD I/O LIBRARY				     ³
'³									     ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³									     ³
'³  1. GetAny(TermSet$,Length%,PosX%,PosY%,Mask$,RetString$,CharToRet$,_     ³
'³	      NumsOnly%)						     ³
'³									     ³
'³     Retrieves a string. Pass in the set of terminating characters,        ³
'³     X position, Y position, Mask, string to be operated on,  a            ³
'³     variable which will contain character that terminated input           ³
'³     of that string, and a boolean to indicate if the string is to         ³
'³     be numeric only.  All characters are converted to upper-case.         ³
'³                                                                           ³
'³     NOTE: if you do not want a mask, pass in "#" for the Mask$.           ³
'³     Otherwise, you must provide entire mask for input  of the string      ³
'³     (ex: "(###)###-####"). Any character that is not a "#" will be        ³
'³     treated as a mask character. Also, the length passed in for           ³
'³     masked strings should be equal to total length of the mask            ³
'³     string.	                                                             ³
'³                                                                           ³
'³  2. GetInteger(TermSet$,Length%,PosX%,PosY%,IntNum%,CharToRet$)           ³
'³                                                                           ³
'³     Retrieves an integer (%) value.  Pass in the set of terminating       ³
'³     characters, length of the field, X position, Y position, integer      ³
'³     value to be operated on, and a variable which will return the         ³
'³     character that terminated input of the integer.                       ³
'³                                                                           ³
'³     NOTE: The number entered cannot be larger than 32,767. Thus, the      ³
'³     length passed into the routine should never exceed five.              ³
'³                                                                           ³
'³  3. GetLongInt(TermSet$,Length%,PosX%,PosY%,IntNum&,CharToRet$)           ³
'³                                                                           ³
'³     Same as GetInteger except it retrieves a long (&) integer.            ³
'³                                                                           ³
'³  4. GetReal(TermSet$,Length%,PosX%,PosY%,RealNum#,CharToRet$)             ³
'³                                                                           ³
'³     Same as GetInteger except it retrieves a real (#) value.  The         ³
'³     field is automatically masked for input of two decimal places.        ³
'³                                                                           ³
'³  5. GetDate(TermSet$,PosX%,PosY%,RetString$,CharToRet$)                   ³
'³									     ³
'³     Retrieves a valid date.  Pass in set of terminating characters,       ³
'³     X position, Y position, string to be operated on, and a variable      ³
'³     which will contain the character that terminated  input of the        ³
'³     date.  The field is automatically masked with "##/##/##". The         ³
'³     routine can not be exited unless a valid date has been entered        ³
'³     or the date is null. Accepts numeric input only.                      ³
'³									     ³
'³  6. GetOneChar(TermSet$,PosX%,PosY%,RetString$,CharToRet$,CharSet$)       ³
'³									     ³
'³     Retrieves one character.  Pass a set of terminating characters,       ³
'³     X position, Y position, string (character) to be operated on,         ³
'³     variable that will return the character that terminated input of      ³
'³     the string, and a set of allowable characters.                        ³
'³									     ³
'³  NOTES: Before any calls can be made to these routines, you must call     ³
'³  InitKBDIO.  This initializes all the variables used by the routines.     ³
'³  Also, the following SHARED statement should be included in any           ³
'³  subroutine that makes calls to these routines:                           ³
'³									     ³
'³  SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$,_     ³
'³         ESC$, AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$,_       ³
'³         PGDN$                                                             ³
'³									     ³
'³  When referring to the extended keys, use the constants above.	     ³
'³									     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

%FALSE =  0
%TRUE  =  1

SUB ReverseVideo
' Puts Text In Reverse Video
  COLOR 0,15
END SUB

SUB NormalVideo
' Returns Text To Normal Video
  COLOR 15,0
END SUB

SUB InitKBDIO
' Initializes all Keyboard I/O global variables
SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$, ESC$,_
       AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$, PGDN$,_
       DaysPerMonth()
DIM    DaysPerMonth(12)
   F1$ =      ";" : F2$ =     "<" : F3$ =      "=" : F4$ = ">": F5$ = "?"
   F6$ =      "@" : F7$ =     "A" : F8$ =      "B" : F9$ = "C": F10$= "D"
   CR$ = Chr$(13) : BS$ = Chr$(8) : ESC$=  Chr$(27)
   ALEFT$ =   "K" : ARIGHT$ =  "M": ADOWN$ =  "P" : AUP$    = "H"
   AHOME$ =   "G" : AEND$   =  "O": PGUP$  =  "I" : PGDN$   = "Q"
   DaysPerMonth[1]= 31 : DaysPerMonth[2] = 28
   DaysPerMonth[3]= 31 : DaysPerMonth[4] = 30
   DaysPerMonth[5]= 31 : DaysPerMonth[6] = 30
   DaysPerMonth[7]= 31 : DaysPerMonth[8] = 31
   DaysPerMonth[9]= 30 : DaysPerMonth[10]= 31
   DaysPerMonth[11]=30 : DaysPerMonth[12]= 31
END SUB

SUB Insert(Obj$,Target$,L%)
' Inserts object string (Obj$) into target string (Target$) at
' specified position (L%).  This routine is used by the EditRealData
' routine to insert numeral in the two decimal places of the real
' value.
LOCAL X%, RightStr$, LeftStr$, R%
  X% = LEN(Target$)
  IF (X% - L%) < 0 THEN ' handle case where Target$ is null
    R% = 0
  ELSE
    R% = X% - L%
  END IF
  RightStr$ = RIGHT$(Target$,R%)  ' pull off right half of Target$
  LeftStr$ = LEFT$(Target$,L%-1)  ' pull off left half of Target$
  Target$ = LeftStr$ + Obj$ + RightStr$ ' Concat Left and Right with Obj$
END SUB

DEF FNInSet(TempCh$,TermSet$)
' This function checks to see if a character (TempCh$) is in a set of
' characters (TermSet$).  If character is in the set, the function
' returns 1 (%True).  Otherwise, it returns 0 (%False)
LOCAL i, Done%
 Done% = %False
 i = 1
 WHILE Done% = %False
   IF MID$(TermSet$,i,1) = TempCh$ THEN
     Done% = %True
     FNInSet = %True
   ELSEIF i = LEN(TermSet$) THEN
     Done% = %True
     FNInSet = %False
   ELSE
     INCR i
   END IF
 WEND
END DEF


SUB GetChar(RChar$,Ch1$,Special%,TermSet$,DataType$,SpecialSet%)
' This routine fetches each individual character.  It analyzes
' each character and determines if the character is allowable for
' that particular data type.  GetChar will not be terminated until
' a legal character has been accepted.  Upon leaving this routine
' Special% will be set to %True if the character is a terminating
' character.  If it is a terminating character, RChar$ will contain
' the the actual terminating character.  If it is not a terminator,
' Ch1$ will contain the character which is to be added to the field
' being operated on.  TermSet$ simply refers to the set of terminating
' characters passed in by the calling procedure.
SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$, ESC$,_
       AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$, PGDN$
LOCAL  Done%,TempSet$
  Special%= %False
  Done%   = %False
  Ch1$    = ""
  RChar$  = ""
  WHILE Done% = %False
     Ch1$ = ""
     WHILE LEN(Ch1$) = 0  ' get a character
       Ch1$ = Inkey$
     WEND
     Ch1$ = UCASE$(Ch1$)
     IF LEN(Ch1$) = 1 THEN  ' if the length = 1, char is not extended char
       IF (FNInSet(Ch1$,ESC$ + CR$) = %True) AND_  '
          (FNInSet(Ch1$,TermSet$) = %True) THEN
         Special% = %True
         Done% = %True
         RChar$ = Ch1$
       ELSE
         IF DataType$ = "R" THEN
           IF SpecialSet% = %True THEN
             TempSet$ = "0123456789" + BS$
           ELSE
             TempSet$ = "0123456789." + BS$
           END IF
           IF FNInset(Ch1$,TempSet$) = %True THEN
             Done% = %True
           END IF
         ELSEIF FNInset(DataType$,"ILN") = %True THEN
           IF FNInSet(Ch1$,"0123456789"+BS$) = %True THEN
             Done% = %True
           END IF
         ELSE
           Done% = %True
         END IF
       END IF
    ELSE
       Ch1$ = MID$(Ch1$,2,1)
       IF FNInSet(Ch1$,TermSet$) = %True THEN
         Done% = %True
         Special% = %True
         RChar$ = Ch1$
       END IF
    END IF
  WEND
END SUB

SUB DrawAny(DataType$,TempInt%,LongInt&,TempChar$,Mask$,MPtr%,Length1%,XPos%,_
            YPos%,Cnt%)
' This routine draws String and integer fields in reverse video.  If the
' String or Integer does not fill the entire length of the field (Length1%),
' the remainder is filled with blanks
LOCAL StringLen%, RemainBlanks%, X%, MaskLen%,SPtr%
   IF ((DataType$ = "I") AND (TempInt% = 0)) OR ((DataType$ = "L") AND_
      (LongInt& = 0)) THEN
      TempChar$ = ""
   END IF
   StringLen% = LEN(TempChar$)
   IF StringLen% <> 0 THEN
      RemainBlanks% = Length1% - StringLen%
   ELSE
      RemainBlanks% = Length1%
   END IF
   Cnt% = StringLen%
   IF LEN(Mask$) = 1 THEN
     FOR X% = 1 TO (Length1%-1)
       Mask$ = Mask$ + "#"
     NEXT X%
   END IF
   SPtr% = 1
   LOCATE YPos%,Xpos%
   FOR MPtr% = 1 TO Length1%
     IF (MID$(Mask$,MPtr%,1) <> "#") THEN
        PRINT MID$(Mask$,MPtr%,1);
     ELSEIF Sptr% <= StringLen% THEN
        PRINT MID$(TempChar$,SPtr%,1);
        INCR SPtr%
     ELSE
        PRINT " ";
     END IF
   NEXT MPtr%
   Done% = %False
   MPtr% = 0
   SPtr% = 0
   FOR X% = 1 TO Length1%
     IF MID$(Mask$,X%,1) = "#" THEN
       INCR Sptr%
     ELSEIF (MID$(Mask$,X%,1) <> "#") AND (Sptr% <= StringLen%) THEN
       INCR MPtr%
     END IF
   NEXT X%
   Xpos% = Xpos% + MPtr% + StringLen%
   MPtr% = StringLen% + MPtr%
   LOCATE Ypos%,Xpos%
END SUB

SUB DrawReal(Length1%,TempReal#,XPos%,Ypos%,TempX%)
' This routine draws real fields in reverse video.  If the real field
' does not fill the entire field, the remainder is filled with blanks
LOCAL T%, Mask$
   TempX% = Xpos% + (Length1%-3)
   IF TempReal# = 0.0 THEN
      LOCATE Ypos%,TempX%
      PRINT ".";
      LOCATE YPos%,TempX%+1
      PRINT "00";
      LOCATE YPos%,Xpos%
      FOR T% = 1 TO (Length1%-3)
        PRINT " ";
      NEXT T%
      LOCATE Ypos%,TempX%
   ELSE
      LOCATE YPos%,Xpos%
      Mask$ = ""
      FOR T% = 1 TO Length1%-3
        Mask$ = Mask$ + "#"
      NEXT T%
      Mask$ = Mask$ + ".##"
      PRINT USING Mask$; TempReal#;
   END IF
   LOCATE Ypos%,TempX%
END SUB

SUB EditAnyData(TermSet$,DataType$,Length1%,Xpos%,Ypos%,Cnt%,Mask$,MPtr%,_
    TempChar$,Ch$)
' This procedure calls GetChar to get individual characters, then takes
' the appropriate action based on the the key pressed.  If the character
' is just one of the allowable characters, it is added to the field (assuming
' there is room to add the character).  If the character is a backspace (BS$),
' the character to the left is deleted.  The routine is used when editing
' strings, integers, and long integers
LOCAL Finished%, CharBack$, TermPushed%,MaskCharCount%, X%, CharOneMasked%
SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$, ESC$,_
       AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$, PGDN$
   MaskCharCount% = 0
   FOR X% = 1 TO Length1%
     IF MID$(Mask$,X%,1) <> "#" THEN
       INCR MaskCharCount%
     END IF
   NEXT X%
   Finished% = %False
   WHILE Finished = %False
      CALL GetChar(CharBack$,Ch$,TermPushed%,TermSet$,DataType$,%False)
      IF TermPushed% = %True THEN
         GOTO GetOut1
      END IF
      IF (Ch$ = BS$) THEN
        IF (MPtr% = 1) AND (MID$(Mask$,1,1) <> "#") THEN
           CharOneMasked% = %True
        ELSE
           CharOneMasked% = %False
        END IF
        IF (Cnt% >0 ) AND (CharOneMasked% = %False)  THEN
           DECR Cnt%
           TempChar$ = LEFT$(TempChar$,Cnt%)
           IF MID$(Mask$,MPtr%,1) <> "#" THEN
             DECR MPtr%
             DECR XPos%
           END IF
           DECR Xpos%
           IF MPtr% >= 1 THEN
             DECR MPtr%
           END IF
           LOCATE Ypos%,Xpos%
           PRINT " ";
           LOCATE Ypos%,Xpos%
        END IF
      ELSE
         IF ((Cnt% + 1) > (Length1% - MaskCharCount%)) OR_
            (MID$(Mask$,MPtr%+1,1) <> "#") AND_
            ((Cnt% + 1) > (Length1% - MaskCharCount%)) THEN
            BEEP
         ELSE
            INCR Cnt%
            TempChar$ = TempChar$ + Ch$
            IF MID$(Mask$,MPtr%+1,1) <> "#" THEN
              INCR MPtr%
              INCR Xpos%
            END IF
            LOCATE Ypos%,Xpos%
            PRINT Ch$;
            INCR Xpos%
            INCR MPtr%
            LOCATE Ypos%,Xpos%
         END IF
      END IF
   WEND
   GetOut1:
   CALL NormalVideo
END SUB

SUB EditRealData(TermSet$,DataType$,Cnt2%,TempReal#,LeftStr$,RightStr$,_
                 Length1%,Ypos%,Xpos%,TempX%,Ch$)
' This procedure calls GetChar to get individual characters, then takes
' the appropriate action based on the the key pressed.  If the character
' is just one of the allowable characters, it is added to the real number
' (assuming there is room to add the character).  If the character is a
' backspace (BS$), the character to the left is deleted.  When backspacing
' in the decimal area, a zero (not a blank) will replace the deleted
' numeral
LOCAL  ValueChanged%,TotChar$,Complete%,Finished%,CharBack$,TermPushed%,_
       Cnt1%,SpecialSet%
SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$, ESC$,_
       AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$, PGDN$

   Finished% = %False
   TempX% = TempX% - Cnt2%
   WHILE Finished% = %False
     CALL GetChar(CharBack$,Ch$,TermPushed%,TermSet$,DataType$,%False)
     IF TermPushed% = %True THEN
        GOTO GetOut2
     END IF
     IF Ch$ = "." THEN
       Cnt1% = 0
       ValueChanged% = %True
       TempX% = Xpos% + Length1% - 2
       LOCATE YPos%,TempX%
       Complete% = %False
       WHILE Complete% = %False
         IF Cnt1% >= 0 THEN
           SpecialSet% = %True
         ELSE
           SpecialSet% = %False
         END IF
         CALL GetChar(CharBack$,Ch$,TermPushed%,TermSet$,DataType$,SpecialSet%)
         IF TermPushed% = %True THEN
            GOTO GetOut2
         END IF
         IF (Ch$ = BS$) AND (Cnt1% = 0) THEN
             Complete% = %True
             LOCATE YPos%,Xpos% + Length1% - 3
             TempX% = Xpos% + Length1% - Cnt2% - 3
         ELSEIF (Ch$ = BS$) AND (Cnt1% > 0) THEN
             DECR TempX%
             DECR Cnt1%
             RightStr$ = LEFT$(RightStr$,Cnt1%)
             LOCATE Ypos%,TempX%
             PRINT "0";
             LOCATE Ypos%,TempX%
         ELSEIF (Ch$ <> BS$) AND (Cnt1% <2) THEN
             INCR Cnt1%
             LOCATE Ypos%,TempX%
             CALL Insert(Ch$,RightStr$,Cnt1%)
             PRINT Ch$;
             IF Cnt1% = 2 THEN
               LOCATE Ypos%,TempX%
         END IF
         INCR TempX%
         END IF
       WEND
     ELSE
       ValueChanged%= %True
       IF (Ch$ = BS$) AND (Cnt2% > 0) THEN
          LOCATE Ypos%,TempX%
          PRINT " ";
          INCR TempX%
          DECR Cnt2%
          LeftStr$ =LEFT$(LeftStr$,Cnt2%)
          LOCATE Ypos%,TempX%
          PRINT LeftStr$
       ELSEIF (Ch$ <> BS$) AND (Cnt2% < Length1%-3) THEN
          LeftStr$ = LeftStr$ + Ch$
          DECR TempX%
          LOCATE Ypos%,TempX%
          INCR Cnt2%
          PRINT LeftStr$;
          LOCATE Ypos%,Xpos%+Length1%-3
       END IF
     END IF
   WEND
   GetOut2:
   IF ValueChanged% = %True THEN
     TotChar$= LeftStr$ + "." + RightStr$
     TempReal# = Val(TotChar$)
   END IF
     CALL NormalVideo
END SUB

SUB ConvertInteger(TempChar$,TempInt%)
' Converts an integer (TempInt%) to a string (TempChar$) to be used
' while editing the integer value
LOCAL X%
   TempChar$ = Str$(TempInt%)
   X% = LEN(TempChar$)
   IF X% > 0 THEN
     TempChar$ = RIGHT$(TempChar$,X%-1)
   END IF
END SUB

SUB ConvertLongInt(TempChar$,TempInt&)
' Converts a long integer (TempInt&) to a string (TempChar$) to be used
' while editing the long integer
LOCAL X%
   TempChar$ = Str$(TempInt&)
   X% = LEN(TempChar$)
   IF X% > 0 THEN
     TempChar$ = RIGHT$(TempChar$,X%-1)
   END IF
END SUB

SUB ConvertReal(TempReal#,LeftStr$,RightStr$,Cnt2%,Length1%)
' Converts a real number (TempReal#) to two strings: LeftStr$, the
' whole part of the number and RightStr$, the decimal portion of
' the number. Cnt2% returns the position in the number where the decimal
' point is.
LOCAL TString$,M%,X%,Done%

   LeftStr$= ""
   RightStr$= ""
   IF TempReal# <> 0 THEN
     TString$ = Str$(TempReal#)
     X% = LEN(TString$)
     TString$ = RIGHT$(TString$,X%-1)
     Done% = %False
     M% = 1
     WHILE Done% = %False
       IF (MID$(TString$,M%,1) = ".") THEN
         Done% = %True
         LeftStr$ = LEFT$(Tstring$,M%-1)
         RightStr$= MID$(Tstring$,M%+1,2)
         Cnt2%= M% - 1
       ELSEIF (M% = LEN(Tstring$)) THEN
         Done% = %True
         LeftStr$ = LEFT$(Tstring$,M%)
         RightStr$= "00"
         Cnt2%= M%
       ELSE
         INCR M%
       END IF
     WEND
   ELSE
     Cnt2%= 0
   END IF
END SUB

SUB ClearReal(YPos%,XPos%,Length1%,TempReal#)
' Clears the reverse video and rewrites the numeral in normal video
LOCAL H%,Mask$
   LOCATE Ypos%,Xpos%
   FOR H% = 1 TO Length1%
     PRINT " ";
   NEXT H%
   IF TempReal# <> 0 THEN
      LOCATE YPos%,XPos%
      Mask$ = ""
      FOR H% = 1 TO Length1%-3
      Mask$ = Mask$ + "#"
      NEXT H%
      Mask$ = Mask$ + ".##"
      PRINT USING Mask$; TempReal#;
   END IF
END SUB

SUB ClearAny(YPos%,Xpos%,Mask$,TempChar$,Length1%)
' Clears the reverse video and re-writes string and integers in
' normal video
LOCAL Z%,TLen%, SLen%
    SLen% = 0
    TLen% = LEN(TempChar$)
    LOCATE YPos%,Xpos%
    FOR Z% = 1 TO Length1%
      IF (MID$(Mask$,Z%,1) <> "#") AND (TLen% > 0) THEN
        PRINT MID$(Mask$,Z%,1);
      ELSEIF (SLen%+1) <= TLen% THEN
        INCR SLen%
        PRINT MID$(TempChar$,Slen%,1);
      ELSE
        PRINT " ";
      END IF
    NEXT Z%
END SUB

SUB GetData(TermSet$,Length1%,Xpos%,Ypos%,Mask$,DataType$,TempChar$,_
            TempReal#,LongInt&,TempInt%,CharBack$)
' This procedure is the main driver of the Keyboard I/O Library.  It
' is called by all library routines (GetInteger, GetReal, etc.)
' It receives all the different data types, but only operates on the
' one specified by the value of DataType$.  Each data type is processed
' as follows:
'
'      1. If field is numeric, convert it to a string
'            ConvertInteger
'            ConvertLongInt
'            ConvertReal
'
'      2. Draw the field in reverse video
'            DrawAny (Strings and Integers)
'            DrawReal
'      3. Edit the field
'            EditAny (Strings and Integers)
'            EditRealData
'      4. Clear the field (re-draw it in normal video)
'            ClearAny (String and Integers)
'            ClearReal
'      5. Convert strings back to numeric values (Integers and Reals)


LOCAL  Finished%,Cnt%,LeftStr$,RightStr$, MPtr%, XStart%
SHARED F1$, F2$, F3$, F4$, F5$, F6$, F7$, F8$, F9$, F10$, CR$, BS$, ESC$,_
       AUP$, ADOWN$, ALEFT$, ARIGHT$, AHOME$, AEND$, PGUP$, PGDN$

   XStart% = XPos%
   Finished%= %False
   CALL ReverseVideo
   IF DataType$ = "R" THEN
      CALL ConvertReal(TempReal#,LeftStr$,RightStr$,Cnt%,Length1%)
      CALL DrawReal(Length1%,TempReal#,XPos%,Ypos%,TempX%)
      CALL EditRealData(TermSet$,DataType$,Cnt%,TempReal#,LeftStr$,_
                        RightStr$,Length1%,Ypos%,Xpos%,TempX%,CharBack$)
      CALL ClearReal(YPos%,XPos%,Length1%,TempReal#)
   ELSEIF DataType$ = "I" THEN
      CALL ConvertInteger(TempChar$,TempInt%)
      CALL DrawAny(DataType$,TempInt%,LongInt&,TempChar$,Mask$,MPtr%,_
                   Length1%,XPos%,YPos%,Cnt%)
      CALL EditAnyData(TermSet$,DataType$,Length1%,Xpos%,Ypos%,Cnt%,Mask$,_
                       MPtr%,TempChar$,CharBack$)
      CALL ClearAny(YPos%,XStart%,Mask$,TempChar$,Length1%)
      IF TempChar$ = "" THEN
          TempInt% = 0
       ELSE
          TempInt% = Val(TempChar$)
       END IF
   ELSEIF DataType$ = "L" THEN
       CALL ConvertLongInt(TempChar$,LongInt&)
       CALL DrawAny(DataType$,TempInt%,LongInt&,TempChar$,Mask$,MPtr%,_
                    Length1%,XPos%,YPos%,Cnt%)
       CALL EditAnyData(TermSet$,DataType$,Length1%,Xpos%,Ypos%,Cnt%,Mask$,_
                        MPtr%,TempChar$,CharBack$)
       CALL ClearAny(YPos%,XStart%,Mask$,TempChar$,Length1%)
       IF TempChar$ = "" THEN
          LongInt& = 0
       ELSE
          LongInt& = Val(TempChar$)
       END IF
   ELSE
       CALL DrawAny(DataType$,TempInt%,LongInt&,TempChar$,Mask$,MPtr%,_
                    Length1%,XPos%,YPos%,Cnt%)
       CALL EditAnyData(TermSet$,DataType$,Length1%,Xpos%,Ypos%,Cnt%,Mask$,_
                        MPtr%,TempChar$,CharBack$)
       CALL ClearAny(YPos%,XStart%,Mask$,TempChar$,Length1%)
   END IF
END SUB

SUB GetReal(TermSet$,Length2%,PosX%,PosY%,RealNum#,CharToRet$)
' This procedure acts as a shell.  It receives the parameters it needs
' to retrieve a real number, then calls GetData. It fills in dummy
' parameters for those fields which are irrelevant
LOCAL BlankInt#,BlankString$,BlankLong& ' dummy parameters
  CALL GetData(TermSet$,Length2%,PosX%,PosY%,"#","R",BlankString$,RealNum#,_
               BlankLong&,BlankInt%,CharToRet$)
END SUB

SUB GetLongInt(TermSet$,Length2%,PosX%,PosY%,IntNum&,CharToRet$)
' This procedure acts as a shell.  It receives the parameters it needs
' to retrieve a Long Integer, then calls GetData. It fills in dummy
' parameters for those fields which are irrelevant
LOCAL BlankInt%,BlankString$,RealNum# ' dummy parameters
  CALL GetData(TermSet$,Length2%,PosX%,PosY%,"#","L",BlankString$,RealNum#,_
               IntNum&,BlankInt%,CharToRet$)
END SUB

SUB GetInteger(TermSet$,Length2%,PosX%,PosY%,IntNum%,CharToRet$)
' This procedure acts as a shell.  It receives the parameters it needs
' to retrieve an Integer, then calls GetData. It fills in dummy
' parameters for those fields which are irrelevant
LOCAL BlankReal#,BlankString$,BlankLong& ' dummy parameters
  CALL GetData(TermSet$,Length2%,PosX%,PosY%,"#","I",BlankString$,BlankReal#,_
               BlankLong&,IntNum%,CharToRet$)
END SUB

SUB GetAny(TermSet$,Length2%,PosX%,PosY%,Mask$,RetString$,CharToRet$,NumOnly%)
' This procedure acts as a shell.  It receives the parameters it needs
' to retrieve a string, then calls GetData. It fills in dummy parameters
' for those fields which are irrelevant
LOCAL BlankReal#,BlankInt%,BlankLong& ' dummy parameters,Dtype$
  IF NumOnly% = %True THEN
    DType$ = "N"
  ELSE
    DType$ = "C"
  END IF
  CALL GetData(TermSet$,Length2%,PosX%,PosY%,Mask$,DType$,RetString$,_
               BlankReal#,BlankLong&,BlankInt%,CharToRet$)
END SUB

SUB GetDate(TermSet$,PosX%,PosY%,RetString$,CharToRet$)
' This procedure will call GetAny with a mask of "##/##/##".  The string
' retrieved is then broken into month, day, year strings. These are
' converted to numbers and are evaluated.  If the date is empty or valid,
' input is terminated. Otherwise the routine will BEEP and require
' the user to re-enter the date.
SHARED DaysPerMonth()
LOCAL Len1%, DayStr$, MonthStr$, YearStr$, Day%,Month%,_
      Year%, GoodDate%

   GoodDate% = %False
   WHILE GoodDate% = %False
      CharToRet$ = " "
      CALL GetAny(TermSet$,8,(PosX%),(PosY%),"##/##/##",RetString$,CharToRet$,_
                  %True)
      IF (LEN(RetString$) = 0) THEN
         GOTO GetOut3
      END IF
      IF LEN(RetString$) = 6 THEN
        MonthStr$ = MID$(RetString$,1,2)
        DayStr$ = MID$(RetString$,3,2)
        YearStr$ = MID$(RetString$,5,2)
        Month% = Val(MonthStr$)
        Day%   = Val(DayStr$)
        Year%  = Val(YearStr$)
        IF ((ABS(1980 + Year%)) Mod 4) = 0 THEN
           DaysPerMonth%[2] = 29
        ELSE
           DaysPerMonth%[2] = 28
        END IF
        IF (Month% <= 12) AND (Month% >= 1) THEN
          GoodDate% = %True
        ELSE
          GoodDate% = %False
        END IF
        IF GoodDate% = %True THEN
           IF (Day% >= 1) AND (Day% <= DaysPerMonth[Month%]) THEN
              GoodDate%= %True
           ELSE
              GoodDate% = %False
           END IF
        END IF
      END IF
      IF GoodDate% = %False THEN
         BEEP
      END IF
   WEND
   GetOut3:
END SUB

SUB GetOneChar(TermSet$,PosX%,PosY%,RetString$,CharToRet$,CharSet$)
' This procedure fetches one character from the keyboard.  It calls
' GetData to get the character, then determines if the character is
' allowable (it is allowable if it is in CharSet$).  If it is an
' allowable character or it is null, input is terminated.  Otherwise,
' the routine will BEEP and the user must re-enter the character
LOCAL BlankReal#, BlankInt%, Length2%, LongInt&, GoodChar%, Ch$, Mask$
   Length2% = 1
   GoodChar% = %False
   WHILE GoodChar% = %False
      CALL GetData(TermSet$,Length2%,(PosX%),(PosY%),"#","C",RetString$,_
                   BlankReal#,LongInt&,BlankInt%,CharToRet$)
      Ch$ = MID$(RetString$,1,1)
      IF (FNInSet(Ch$,CharSet$) = %True) OR (RetString$ = "") THEN
         GoodChar% = %True
      ELSE
         RetString$ = ""
         BEEP
      END IF
   WEND
END SUB
