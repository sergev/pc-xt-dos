'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³                             WHEREIS.BAS                                   ³
'³                             VERSION 1.0                                   ³
'³                                                                           ³
'³			       Turbo Basic				     ³
'³		(C) Copyright 1987 by Borland International		     ³
'³                                                                           ³
'³                                                                           ³
'³ System Requirements:                                                      ³
'³   - DOS Version 2.0 or later                                              ³
'³   - 320K                                                                  ³
'³                                                                           ³
'³   This  program is a  utility that  will perform a search  for a file or  ³
'³ group of files that the user specifies.  The program is designed to dem-  ³
'³ onstrate a couple of features that are unique to Turbo Basic. One of the  ³
'³ features is a  concept called "recursion."  To put it briefly, recursion  ³
'³ means  that a procedure  or function  may call  itself!  The other Turbo  ³
'³ Basic feature that  WHEREIS demonstrates is  INLINE  procedures.  INLINE  ³
'³ procedures are Turbo Basic's method	of providing an interface to assem-  ³
'³ bler language routines.  If you are interested in using routines written  ³
'³ in assembly language,  we suggest that you  read Appendix C of the Turbo  ³
'³ Basic  manual and study this program as an example.                       ³
'³                                                                           ³
'³   In order to use this program do the following:                          ³
'³                                                                           ³
'³     1) Using the MicroSoft Assembler(MASM.EXE), linker(LINK.EXE) and      ³
'³        conversion program(EXE2BIN.EXE) compile and link the files:        ³
'³          SETDTA.ASM                                                       ³
'³          GETDTA.ASM                                                       ³
'³          GETDIR.ASM                                                       ³
'³          GETDRIVE.ASM                                                     ³
'³        In order to compile them use the following commands:               ³
'³          A:>MASM filename;     - creates a .OBJ file                      ³
'³          A:>LINK filename;     - creates a .EXE file                      ³
'³          A:>EXE2BIN filename   - creates the .BIN file used by WHEREIS    ³
'³                                                                           ³
'³     2) Load WHEREIS.BAS into Turbo Basic and compile it to a .EXE file:   ³
'³          a) At the DOS prompt type:                                       ³
'³              A:>TB                                                        ³
'³          b) In the File pulldown menu select the Main file option and     ³
'³             enter WHEREIS as the main file.                               ³
'³          c) Specify that you want to compile to a .EXE file in the 	     ³
'³             OPTIONS pulldown menu under Compile to.    		     ³
'³          c) Compile the program by selecting Compile from the Main Menu   ³
'³                                                                           ³
'³     3) Exit Turbo Basic and at the DOS prompt type:                       ³
'³          A:>WHEREIS filespec                                              ³
'³        where filespec is the name of the file you want to look for.       ³
'³        Note that you may use any legal DOS filename including wildcards   ³
'³        like ? and * and drive names like A or C. An example might be:     ³
'³          A:>WHEREIS C:\TB\*.BAS                                           ³
'³                                                                           ³
'³                                                                           ³
'³   If  you would  like more  technical information about how this program  ³
'³ works, we suggest the following sources:                                  ³
'³                                                                           ³
'³   Programmer's Guide to the IBM-PC - Published by MicroSoft Press	     ³
'³   Advanced MS-DOS                  - Published by MicroSoft Press         ³
'³   DOS Technical Reference Manual   - Published by IBM                     ³
'³                                                                           ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


$STACK 32767	' set up plenty of stack space since this is a recursive
		' program

' named constant declarations
%DOSPathLength = 64		' maximum length for DOS path
%True	       = -1		' boolean flag
%False	       = 00		' boolean flag
%DTASize       = 43		' size of DOS Data Transfer Area
%FileNameOfs   = 31		' offset into DTA of file name
%FileNameLen   = 12		' length of DOS file name
%DosCall       = &H21		' DOS Function call
%Directory     = &H10		' attribute for DOS sub-directory



'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ MAIN PROGRAM ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿

  CLS
  CALL Initialize			' initialize global data
  CALL GetFileName(Path$, FileSpec$)	' get file spec. and starting path
  CALL FindFiles(FNStripWhiteSpace$(Path$), FileSpec$) ' do recursive search
  CALL ByeBye				' call exit routine

'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


$INCLUDE "REGNAMES.INC" ' named constant definitions for register handling
$INCLUDE "WHEREIS.INC"	' include file of routines to get input from the
			' user and search for the files

SUB Initialize
' This procedure saves the current drive and directory since we want the
' program to return to that place when it is done searching. This routine
' also stores the address of the current Data Transfer Area. We must do this
' because if we don't restore the address of the Data Transfer Area, other
' programs may not be able to do file I/O. Finally, we set up a couple of
' key board event traps so the user can pause or break the program while
' it is executing.

  SHARED OldDTASegment%, OldDTAOffset%, FileMask$, HomeDirectory$
  LOCAL Drive%

  HomeDirectory$ = SPACE$(%DOSPathLength)	' allocate space for path
  CALL GetDrive(Drive%)				' get the current drive
  CALL GetDir(0%, HomeDirectory$)		' get the current directory

  ' save the current Drive\Directory in HomeDirectory$
  HomeDirectory$ = CHR$(Drive% + &H41) + ":\" + HomeDirectory$

  CALL GetDTA(OldDTASegment%, OldDTAOffset%)	' save the current DTA address

  FileMask$ = "????????.???" + CHR$(0)	' create mask for DOS directory
					' searching
END SUB ' procedure Initialize


SUB ByeBye
' This procedure is the route by which the program is always terminated.
' Upon termination of the program we must restore the original drive\
' directory and we must restore the original Disk Transfer Area.

  SHARED OldDTASegment%, OldDTAOffset%, HomeDirectory$

  CALL ChangeDir(HomeDirectory$)	' change back to original directory

  CALL SetDTA(OldDTASegment%, OldDTAOffset%, "") ' restore original DTA

  ' tell user we're done
  LOCATE 24,1 : PRINT SPACE$(80); : LOCATE 24,1 : PRINT "Search complete!";
  BEEP

  END			' terminate execution

END SUB ' procedure ByeBye
