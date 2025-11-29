'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                               MC.BAS                          	     ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³                           MODULE: MC1.INC                                 ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ DESCRIPTION: Miscellaneous commands and utilities (Keyboard,screen,	     ³
'³              Autocalc)						     ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


SUB ClearStat
' ClearStat clears the status line at line number @StatusLine%

   LOCATE %StatusLine ,1                    ' Go to the status line location
   PRINT LEFT$(SPACE$(%ScreenSize ),79);    ' and write blanks over anything
   					    ' there
END SUB

SUB Msg( Buffer$ )
' Msg displays a message on the status line

   CALL ClearStat                         ' Clear out anything that is on the
                                          ' status line
   LOCATE %StatusLine ,1                  ' Go there
   Buffers$=LEFT$(Buffer$,%ScreenSize )   ' Make sure the string will fit
   PRINT Buffer$;                         ' print out the message

END SUB

SUB Flash(X%,Buffer$,Blink%)
' Display a temporary flashing message to the user.

    Buffers$=LEFT$(Buffer$,%ScreenSize ) ' Make sure the string will fit
    LOCATE %FlashLine ,X%
    IF Blink% THEN
      CALL BlinkVideo
      PRINT Buffer$;
      WHILE NOT INSTAT : WEND
    ELSE
      CALL NormVideo
      PRINT Buffer$;
    END IF

END SUB

DEF FNMASK$(Fw%,Nd%)
    IF Nd%<=-1 THEN
       FNMASK$="."+STRING$(FW%-5,"#")+"^^^^"
    ELSEIF Nd%=0 THEN
       FNMASK$=STRING$(FW%,"#")
    ELSE
       FNmask$=STRING$(FW%-Nd%-1,"#")+"."+STRING$(Nd%,"#")
    END IF
END DEF

SUB Auto
' Toggles AutoCalc on AND off

    shared Autocalc%

    AutoCalc% = NOT AutoCalc%
    CALL DISPAUTO               ' call the routine that displays AutoCalc
    				' state
END SUB

SUB ReadKBD(RetChar$)
' This function reads a keystroke from the keyboard and returns 1 OR 2
' character string.

    DO
      RetChar$ = INKEY$          ' get the keyboard input
    LOOP UNTIL RetChar$<>""

END SUB

SUB IBMCh( Ch$ )
' IBMCh returns the control character associated with the function keys.
' (ie. right arrow will be changed TO a ^D.

    IF ( asc(Ch$) = 0 ) THEN         ' Check to see if the character is null
      SELECT CASE asc(MID$(Ch$,2,1)) ' If so then a function key was hit and
                                     ' we need to check the scan code
        CASE 72                      ' Up Arrow
          Ch$ = CHR$(5)
        CASE 80                      ' Down Arrow
          Ch$ = CHR$(24)
        CASE 77                      ' Right Arrow
          Ch$ = CHR$(4)
        CASE 75                      ' Left Arrow
          Ch$ = CHR$(19)
        CASE 83                      ' Delete Key
          Ch$ = CHR$(7)
        CASE 82                      ' Insert Key
          Ch$ = CHR$(22)
        CASE 71                      ' Home key
          Ch$ = CHR$(1)
        CASE 79                      ' END key
          Ch$ = CHR$(6)
        CASE 60                      ' F2 key
          Ch$ = CHR$(%EditKey )
        CASE 15                      ' Shift Tab
          Ch$ = CHR$(15)
        CASE ELSE                    ' Everything else
          Ch$ = CHR$(0)
      END SELECT
    END IF
END SUB

SUB ClrEol
' This procedure clears any text from the current cursor position
' to the end of the line on the screen.

    LOCAL OldX%

    OldX% = POS
    PRINT SPACE$(80-Oldx%-1);
    LOCATE ,OldX%

END SUB

DEF FNUnsign&(V&)
' to prevent negative value
    IF V&<0 THEN FNUnsign&=V&+65536 ELSE FNUnsign&=V&
END DEF

SUB LowVideo
   COLOR %LowColor ,0
END SUB

SUB NormVideo
   COLOR %NormColor ,0
END SUB

SUB BlinkVideo
  COLOR %BlinkColor ,0
END SUB

SUB InvVideo
  COLOR 0,7
END SUB

DEF FNgetcmd$
'Get the command line parameter
    STATIC Cmdi%
    LOCAL  Cmdline$,Cmdchar$,Cmdword%
    Cmdline$="" : Cmdword%=0
    IF Cmdi%=0 THEN incr Cmdi%
   DO
      Cmdchar$=MID$(command$,Cmdi%,1)
      IF Cmdchar$<>" " THEN
          Cmdline$=Cmdline$+Cmdchar$ : Cmdword%=1
      END IF
      incr Cmdi%
   LOOP UNTIL Cmdchar$="" OR (Cmdword%=1 AND Cmdchar$=" ")
   FNgetcmd$=Cmdline$
END DEF

DEF FNActivityTime$(BeginTimer)

  LOCAL t$,t,h

  t=INT(TIMER-BeginTimer)
  IF t>0 THEN
    h=INT(t/3600.0)
    IF h>0 THEN
      t$=t$+STR$(h)+" Hour"
      IF h>1 THEN t$=t$+"s"
    END IF
    t=t-h*3600.0
    h=INT(t/60.0)
    IF h>0 THEN
      t$=t$+STR$(h)+" Minute"
      IF h>1 THEN t$=t$+"s"
    END IF
    h=t-h*60.0
    IF h>0 THEN
      t$=t$+STR$(h)+" Second"
      IF h>1 THEN t$=t$+"s"
    END IF
    FNActivityTime$=t$
  END IF
END DEF
