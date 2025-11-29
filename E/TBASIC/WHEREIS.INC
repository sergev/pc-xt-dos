'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                            WHEREIS.BAS                                    ³
'³                            VERSION 1.0                                    ³
'³                                                                           ³
'³                         MODULE: WHEREIS.INC                               ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³ Procedures and Functions in this module:                                  ³
'³   The procedures and functions in this module are divided into three      ³
'³   groups:                                                                 ³
'³     1) Input Routines                                                     ³
'³     2) INLINE Assembler Routines                                          ³
'³     3) Directory Searching Routines                                       ³
'³                                                                           ³
'³ 1) INPUT ROUTINES:                                                        ³
'³   DEF FNIsDir%(FileSpec$)  ' returns whether or not the parameter is      ³
'³                            ' the name of a directory			     ³
'³   DEF FNIsLegalFileSpec%(FileSpec$)  ' returns if the file specification  ³
'³                                      ' given by the user is legal	     ³
'³   SUB GetFileName(Path$, FileSpec$)  ' asks the user for the name of the  ³
'³                                      ' file to search for		     ³
'³   DEF FNMassage$(Spec$)    ' manipulates a file name or specification     ³
'³                            ' so there are no spaces or *'s                ³
'³                                                                           ³
'³ 2) INLINE ASSEMBLER ROUTINES:                                             ³
'³   SUB SetDTA INLINE        ' sets the new address for the Data Transfer   ³
'³                            ' Area					     ³
'³   SUB GetDTA INLINE        ' gets the address of the current Data	     ³
'³                            ' Transfer Area				     ³
'³   SUB GetDir INLINE        ' returns the current directory		     ³
'³   SUB GetDrive INLINE      ' returns the current drive		     ³
'³                                                                           ³
'³ 3) DIRECTORY SEARCHING ROUTINES                                           ³
'³   SUB GetStringAddr(Segment%, Offset%, S$) ' returns the address of the   ³
'³                                            ' string passed to it	     ³
'³   DEF FNFindFirst%  ' finds the first file in a directory		     ³
'³   DEF FNFindNext%   ' finds the next entry in a directory		     ³
'³   DEF FNFoundMatch%(FSpec$, DTA$) ' returns whether or not the file spec. ³
'³                                   ' matches the last entry found in the   ³
'³                                   ' directory			     ³
'³   DEF FNStripWhiteSpace$(S$)      ' returns a string without any spaces   ³
'³                                   ' or null characters in it		     ³
'³   SUB FindFiles(Path$, FileSpec$) ' this is the recursive procedure that  ³
'³                                   ' actually searches for the user's file ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


'ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ INPUT ROUTINES ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

DEF FNIsDir%(FileSpec$)
' This function returns whether or not FileSpec$ is the name of a
' directory. In order to do this we set up a local error handler to trap
' any run-time errors generated in this routine and then try to change
' directories to the directory specified by the user. If an error occurs
' we know that the directory doesn't exist. We trap the run-time error
' and return false in this case. Otherwise, we return true.

  LOCAL Drive%, OldDir$

  ' we need to strip the last "\" off the file spec so TB's CHDIR statement
  ' won't give an I/O error.

  ' check if last char in file spec. is a "\" and if it's > 3
  IF (RIGHT$(FileSpec$,1) = "\") AND (LEN(FileSpec$) > 3) THEN
    FileSpec$ = LEFT$(FileSpec$,LEN(FileSpec$) - 1)	' remove "\"
  END IF

  OldDir$ = SPACE$(%DosPathLength)	' allocate space for directory
  CALL GetDrive(Drive%)			' get the current path
  CALL GetDir(0%, OldDir$)		' get the current drive
  OldDir$ = CHR$(Drive% + &H41) + ":\" + OldDir$        ' store drive\path

  ON ERROR GOTO DirErrorHandler		' set up error trap
  CHDIR FileSpec$			' attempt to change directories

  FNIsDir% = %True ' if this is executed then the directory existed
  CHDIR OldDir$    ' change back to original directory
  GOTO ExitIsDir

  DirErrorHandler:
    RESUME NotADir  ' clear error and continue execution

  NotADir:
    FNIsDir% = %False  ' file spec. is not a directory name

  ExitIsDir:
    ON ERROR GOTO 0

END DEF ' function FNIsDir%


DEF FNIsLegalFileSpec%(FileSpec$)
' This function returns a value indicating if the file specification
' passed to it is legal. In order to determine this we compare each character
' in the file specification with a set of illegal characters. If there are
' any illegal characters in the file specification we return false. If there
' are no illegal characters we then check to make sure that there aren't
' more than eight characters in the file name and three characters in the
' extension.

  LOCAL Illegal$, DotPos%

  ' initialize illegal file characters
  Illegal$ = "/\[]:|<>+=;," + CHR$(34)
  FOR I% = 0 TO &H20
    Illegal$ = Illegal$ + CHR$(I%)
  NEXT I%

  FOR I% = 1 TO LEN(Illegal$)		' for each character in file spec.
    IF INSTR(FileSpec$, MID$(Illegal$, I%, 1)) <> 0 THEN
      FNIsLegalFileSpec% = %False	' assign function its result
      EXIT DEF		' no need to go further so exit function
    END IF
  NEXT I%

  FNIsLegalFileSpec% = %True		' assign function its result

  DotPos% = INSTR(FileSpec$, ".")	' get position of "." in file spec.
  IF DotPos% = 0 THEN			' no file extension
    IF LEN(FileSpec$) > 8 THEN		' file name is too long
      FNIsLegalFileSpec% = %False	' assign function its result
    END IF
  ELSEIF (DotPos% > 8) OR ((LEN(FileSpec$) - DotPos%) > 3) THEN
    FNIsLegalFileSpec% = %False		' file name is too long
  END IF

END DEF ' function FNIsLegalFileSpec%


SUB GetFileName(Path$, FileSpec$)
' This procedure returns the file to search for and where to begin looking.
' First it checks to see if the user specified the information on the DOS
' command line. If not it prompts the user for the information.

  LOCAL CurrentDrive%, Position%
  LOCAL TempStr$, TempPath$, CurrentDir$, Drive%

  CurrentDrive% = 0
  IF LEN(COMMAND$) > 0 THEN
    TempStr$ = COMMAND$    ' get command line parameter
  ELSE
    INPUT "Please enter the path and file specification: ",TempStr$
  END IF

  IF (LEN(TempStr$) >= 3) AND (MID$(TempStr$,2,1) = ":") AND _
     (NOT (MID$(TempStr$,3,1) = "\")) THEN      ' start in current directory
    ' user specified drive but not directory so assume the root directory
    TempStr$ = LEFT$(TempStr$, 2) + "\" + RIGHT$(TempStr$, LEN(TempStr$) - 2)
  END IF

  IF NOT FNIsDir%(TempStr$) THEN
    DO			    ' find last backslash in file spec.
      Position% = INSTR(1, TempStr$, "\")
      ' save first part of spec.
      TempPath$ = TempPath$ + MID$(TempStr$, 1, Position%)
      ' get what's left of string
      TempStr$ = RIGHT$(TempStr$, LEN(TempStr$) - Position%)
    LOOP UNTIL Position% = 0
  ELSE
    TempPath$ = TempStr$		' user didn't give a file to search
    TempStr$ = "*.*"			' for so list them all
  END IF

  IF TempPath$ = "" THEN TempPath$ = "\"        ' user didn't give path

  ' check if last char in file spec. is a "\" and if it's > 3
  IF (RIGHT$(TempPath$,1) = "\") AND (LEN(TempPath$) > 3) THEN
    TempPath$ = LEFT$(TempPath$,LEN(TempPath$) - 1)	' remove last "\"
  END IF

  Path$ = TempPath$	' we now have the specified path and file spec.
  FileSpec$ = TempStr$

  IF NOT FNIsDir%(Path$) THEN		' verify that path exists
    PRINT "You specified a non-existent drive\path...";
    PRINT "Program aborting!"
    CALL ByeBye				' call abort routine
  END IF

  IF NOT FNIsLegalFileSpec%(FileSpec$) THEN
    PRINT "Invalid file specification"
    PRINT "Program aborting!"
    CALL ByeBye		       ' call abort routine
  END IF
  LOCATE 2,1
  PRINT USING "Searching for: &  Starting in directory: & ";FileSpec$, Path$
  PRINT
END SUB ' procedure GetFileName


DEF FNMassage$(Spec$)
' This function expands a filename into it's maximum size inserting
' "?"s wherever appropriate. This makes it much easier to compare the
' file specification given by the user to the file names returned by
' the directory search routines.

  LOCAL StarPos%, DotPos%, TmpStr$	' declare local variables

  StarPos% = INSTR(Spec$, "*")		' get position of first '*' 
  DotPos%  = INSTR(Spec$, ".")		' get position of '.' 

  ' first fix up filename part of file specification
  SELECT CASE StarPos%
    CASE = 0			' There is no '*' in the file name
      IF DotPos% <> 0 THEN	' there is a '.' indicating a file extension
	TmpStr$ = LEFT$(Spec$, DotPos% - 1) + STRING$(9 - DotPos%, "?") + "."
      ELSE			' no "*" and no "." in the file spec
	TmpStr$ = FNStripWhiteSpace$(Spec$)	' strip any spaces or nulls
	TmpStr$ = TmpStr$ + STRING$(8 - LEN(TmpStr$), "?") + "."
      END IF
    CASE = 1			' "*" is first character in file name
      TmpStr$ = "????????."
    CASE > 1
      IF StarPos% > DotPos% THEN	' * is in extension not the file name
	TmpStr$ = LEFT$(Spec$, DotPos% - 1) + STRING$(9 - DotPos%, "?") + "."
      ELSE
	TmpStr$ = LEFT$(Spec$, StarPos% - 1) + _
		  STRING$(9 - StarPos%, "?") + "."
      END IF
  END SELECT

  ' now fix up the file spec's extension

  IF DotPos% <> 0 THEN				' "." exists in file name
    StarPos% = INSTR(DotPos%, Spec$, "*") - DotPos%
  ELSE
    StarPos% = 0
  END IF

  SELECT CASE StarPos%
    CASE <= 0				' there is no "*" in the extension
      IF DotPos% <> 0 THEN
	IF LEN(Spec$) > DotPos% THEN		' there are chars after '.' 
	  Spec$ = FNStripWhiteSpace$(Spec$)
	  TmpStr$ = TmpStr$ + _
			MID$(FNStripWhiteSpace$(Spec$), DotPos% + 1, _
			LEN(Spec$) - DotPos%) + STRING$(3 - (LEN( _
			FNStripWhiteSpace$(Spec$)) - DotPos%), "?")
	ELSE				  ' "." is last char of file spec.
	  TmpStr$ = TmpStr$ + "???"
	END IF
      ELSE			      ' there isn't a "." in the file spec.
	TmpStr$ = TmpStr$ + "???"
      END IF
    CASE = 1				' star is first char of extension
      TmpStr$ = TmpStr$ + "???"		' so we ignore anything after it
    CASE > 1				' there are characters before "*"
      TmpStr$ = TmpStr$ + _		' so get them and expand *
		MID$(Spec$, DotPos% + 1, StarPos% - (DotPos% + 1)) + _
		STRING$(3 - (StarPos% - DotPos% + 2), "?")
  END SELECT

  FNMassage$ = UCASE$(TmpStr$)
END DEF ' function FNMassage$


'ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ INLINE ASSEMBLER ROUTINES ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

SUB SetDTA INLINE ' (Segment%, Offset%, DTA$) - required parameter list
' This procedure sets the current Data Transfer Area. The procedure must be
' passed three parameters. The first two are the segment and the offset of
' the new DTA and the third is a string variable that will be used as the DTA.
' The reason all three parameters must be used is because the procedure can
' be called in either one of two ways. If both the Segment and Offset values
' are equal to zero then the DTA will be set to the location of the string
' descriptor. However, if either the segment or the offset is not equal to
' zero then then the DTA will be set to the address specified by the first
' two parameters.

$INLINE "SETDTA.BIN"		' inline code file

END SUB ' procedure SetDTA


SUB GetDTA INLINE ' (Segment%, Offset%) - required parameter list
' This procedure gets the address of the current Data Transfer Area. The
' procedure must be passed two parameters that will store the Segment and
' Offset of the current DTA.

$INLINE "GETDTA.BIN"		' inline code file

END SUB


SUB GetDir INLINE ' (Directory$) - required parameter list
' This procedure returns the current directory in the string parameter
' passed to it. Note that before calling this routine the string must have
' space allocated to it or this procedure will cause the string segment to
' be corrupted. The call should look like:
' 
'   Directory$ = SPACE$(%DOSPathLength) ' allocate space for string
'   CALL GetDir(Directory$)             ' get current directory

$INLINE "GETDIR.BIN"		' inline code file

END SUB ' procedure GetDir


SUB GetDrive INLINE ' (Drive%) - required parameter list
' This procedure returns a number representing the current drive in the
' integer parameter passed to it. A 0 represents drive A a 1 drive B, etc.

$INLINE "GETDRIVE.BIN"

END SUB ' procedure GetDrive



'ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ DIRECTORY SEARCHING ROUTINES ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

SUB GetStringAddr(Segment%, Offset%, S$)
' This procedure returns the address of the string passed to it. In order to
' do this we must do two things; First we must get the segment of the string
' by doing a PEEK of the first two bytes of the Turbo Basic data segment.
' Then we must look at the string descriptor for the string to determine the
' offset of the string. The second step proves to be a bit more complex than
' the first.

  LOCAL Ofs%

  Segment% = PEEK(0) + (256 * PEEK(1)) ' get the location of the string segment

  DEF SEG = VARSEG(S$)	' set default segment to location of string descriptor
  Ofs% = VARPTR(S$)	' get offset of string descriptor

  Offset% = PEEK(Ofs% + 2) + (256 * PEEK(Ofs% + 3))

  DEF SEG		' restore Turbo Basic data segment

END SUB ' procedure GetStringAddr


SUB ChangeDir(Directory$)
' This procedure changes the current directory to the one specified in the
' parameter. Note that if a drive specification is given, the routine also
' changes the current drive.

  LOCAL Segment%, Offset%, Drive%

  IF MID$(Directory$,2,1) = ":" THEN	' need to change drives
    REG %AX, &H0E00	' DOS service to change drives
    ' put destination drive in DL
    REG %DX, (ASC(UCASE$(LEFT$(Directory$,1))) - &H41) AND &H00FF
    CALL INTERRUPT &H21 ' make DOS service call
  END IF

  CHDIR Directory$		' change the current directory

END SUB 'ChangeDir


DEF FNFindFirst%
' This function searches for the first file entry in a directory. It returns
' the error code that DOS returned in AX.

  SHARED FileMask$
  LOCAL ErrorCode%, MaskOfs%, MaskSeg%

  CALL GetStringAddr(MaskSeg%, MaskOfs%, FileMask$) ' get address of mask

  REG %DS, MaskSeg%	  ' segment of ASCIIZ string
  REG %DX, MaskOfs%	  ' offset of ASCIIZ string
  REG %CX, &B00111111	  ' specify attributes to search for (ALL)

  REG %AX, &H4E00
  CALL INTERRUPT %DosCall	' do DOS function call

  FNFindFirst% = REG(%AX)	' return resulting code

END DEF ' function FNFindFirst%


DEF FNFindNext%
' This function finds the next file entry in the current directory. It
' returns the error code that DOS returned in AX.

  SHARED FileMask$
  LOCAL ErrorCode%, MaskOfs%, MaskSeg%

  CALL GetStringAddr(MaskSeg%, MaskOfs%, FileMask$)	' get address of mask
  REG %DS, MaskSeg%	  ' segment of ASCIIZ string
  REG %DX, MaskOfs%	  ' offset of ASCIIZ string
  REG %CX, &B00111111	  ' specify attributes to search for (ALL)

  REG %AX, &H4F00
  CALL INTERRUPT %DosCall	' do DOS function call

  FNFindNext% = REG(%AX)	' return DOS code

END DEF ' function FNFindNext%


DEF FNFoundMatch%(FSpec$, DTA$)
' This function returns a boolean value indicating if the file found by
' the call to FNFindFirst% or FNFindNext% matches the users search
' specifications. It determines a match by copying the file entry found out
' of the DTA and comparing it with the user's file spec. Note that the
' file entries "." and ".." are never returned as matches since they are
' of little use to anyone looking for a file.

  LOCAL TmpStr$, TmpSpec$, TmpFile$, NameCount%, ExtCount%

  TmpStr$ = MID$(DTA$, %FileNameOfs, %FileNameLen)
  IF LEFT$(TmpStr$,1) = "." OR LEFT$(TmpStr$,2) = ".." THEN   ' if "." or ".."
    FNFoundMatch% = %False			' directories then skip
  ELSE	' it's not "." or ".."
    TmpSpec$ = FNMassage$(FSpec$)	' massage file names
    TmpFile$ = FNMassage$(TmpStr$)
    FNFoundMatch% = %True		' assume it's a match

    ' figure out last character to look at - either last non ? char or ...
    FOR Count% = 1 TO 8		' find last non-? in file name
      IF MID$(TmpSpec$, Count%, 1) <> "?" THEN
	NameCount% = Count%
      END IF
    NEXT Count%
    IF NameCount% = 0 THEN NameCount% = 8      ' there weren't any non-? chars
    FOR Count% = 10 TO 12	' find last non-? in file extension
      IF MID$(TmpSpec$, Count%, 1) <> "?" THEN
	ExtCount% = Count%
      END IF
    NEXT Count%
    IF ExtCount% = 0 THEN ExtCount% = 3  ' there weren't any non-? chars

    FOR Count% = 1 TO NameCount%
      IF MID$(TmpSpec$, Count%, 1) <> MID$(TmpFile$, Count%, 1) THEN
	IF MID$(TmpSpec$, Count%, 1) <> "?" THEN
	  FNFoundMatch% = %False	' it's not a match so leave
	  EXIT DEF
	END IF
      END IF
    NEXT Count%

    FOR Count% = 1 TO ExtCount%
      IF MID$(TmpSpec$, Count%, 1) <> MID$(TmpFile$, Count%, 1) THEN
	IF MID$(TmpSpec$, Count%, 1) <> "?" THEN
	  FNFoundMatch% = %False	' it's not a match so leave
	  EXIT DEF
	END IF
      END IF
    NEXT Count%
  END IF
END DEF ' function FNFoundMatch%


DEF FNStripWhiteSpace$(S$)
' This function deletes any spaces or null characters from the string passed
' to it and returns the resulting string

  LOCAL SpacePos%, I%

  I% = 1		' initialize counter
  WHILE I% <= LEN(S$)	' while we haven't reached the end of the string
    IF (MID$(S$,I%,1) = CHR$(32)) OR (MID$(S$,I%,1) = CHR$(00)) THEN
      S$ = LEFT$(S$, I% - 1) + RIGHT$(S$, LEN(S$) - I%) ' delete the character
    ELSE
      INCR I%	' just increment the counter to the next char in the string
    END IF
  WEND
  FNStripWhiteSpace$ = S$	' return the new string
END DEF ' FNStripWhiteSpace$


SUB FindFiles(Path$, FileSpec$)
' This procedure is the main routine in the program. It is passed the files
' specification to search for and where to begin searching. It then does
' a recursive search - searching in any sub-directories of the directory
' specified by the user. When it has found the last entry in the directory
' specified by the user the program terminates.


  SHARED FileMask$
  LOCAL DTASegment%, DTAOffset%, ErrorCode%, Segg%, Ofss%
  LOCAL InputStr$, DTA$

  DTA$ = SPACE$(%DTASize) ' allocate space for the Data Transfer Area
			  ' The DOS DTA looks like:
			  '  1..21 - reserved for DOS
			  '     22 - File Attribute
			  '          Note that the file attribute byte is set
			  '          up in the following manner:
			  '            BIT         Meaning
			  ' least sig-  1   - The file is marked READ ONLY
			  ' nificant    2   - Indicates a HIDDEN file
			  '             3   - Indicates a system file
			  '             4   - This entry is teh VOLUME LABEL
			  '             5   - Entry is a SUB-DIRECTORY
			  '             6   - Indicates an ARCHIVE bit
			  ' 23..24 - File's Time
			  ' 25..26 - File's Date
			  ' 27..28 - Low Word of File Size
			  ' 29..30 - High Word of File Size
			  ' 31..43 - File Name including period of file
			  '          extension exists and terminated bu NULL
  PRINT "Searching: ";Path$

  CALL ChangeDir(Path$) ' change to directory to be searched

  DTASegment% = 0	' set both offset and segment to 0 in order to
  DTAOffset%  = 0	' force the SetDTA routine to use the string variable
  CALL SetDTA(DTASegment%, DTAOffset%, DTA$)	' set the new DTA location

  MID$(DTA$, %FileNameOfs, %FileNameLen) = SPACE$(12) ' initialize DTA
  ErrorCode% = FNFindFirst%
  IF ErrorCode% = 0 THEN
    IF FNFoundMatch%(FileSpec$, DTA$) THEN
      PRINT USING "Found First Match: & ";Path$ + _
					MID$(DTA$,%FileNameOfs,%FileNameLen)
    END IF
    MID$(DTA$, %FileNameOfs, %FileNameLen) = SPACE$(12) ' initialize DTA
    ErrorCode% = FNFindNext%
    WHILE ErrorCode% = 0
      IF INSTAT THEN
	InputStr$ = INKEY$
	IF LEFT$(InputStr$,1) = CHR$(27) THEN	' if user pressed escape
	  CALL ByeBye
	ELSE
	  WHILE NOT INSTAT : WEND		' wait for user to press key
	  InputStr$ = INKEY$		' gobble key stroke so it isn't
	END IF				' processed next time through loop
	InputStr$ = ""
      END IF
      IF RIGHT$(Path$,1) = "\" THEN BackSpace$ = "" ELSE BackSpace$ = "\" 

      IF MID$(DTA$,22,1) = CHR$(%Directory) AND _
		(NOT (MID$(DTA$,%FileNameOfs,1) = "." OR _
		      MID$(DTA$,%FileNameOfs,2) = "..")) THEN
	CALL FindFiles(FNStripWhiteSpace(Path$ + BackSpace$ + _
		       MID$(DTA$,%FileNameOfs,%FileNameLen)), FileSpec$)
	CALL SetDTA(DTASegment%, DTAOffset%, DTA$) ' set the new DTA location
      ELSE
	IF FNFoundMatch%(FileSpec$, DTA$) THEN
	  PRINT USING " & ";Path$ + BackSpace$ + _
			    MID$(DTA$,%FileNameOfs,%FileNameLen)
	END IF
      END IF
      MID$(DTA$, %FileNameOfs, %FileNameLen) = SPACE$(12) ' initialize DTA
      ErrorCode% = FNFindNext%
    WEND
  END IF
END SUB ' procedure FindFiles
