REM   Program Title: RUNEC
REM   
REM   You may use this batch file to create a program and debug it.
REM   This batch file should be called by typing: A:RUNEC FILENAME.
REM   Filename is the name you want your program to be called.  A .COB
REM   extension will automatically be added to your file.
REM
REM   First you will enter EDLIN allowing you to type in your program.
REM   Then the COBOL compiler is used to compile your program.  Next
REM   you are placed back into EDLIN to correct any errors that may
REM   have occurred.  This cycle is continuously repeated.  
PAUSE...ADDITIONAL INSTRUCTIONS FOLLOW
REM
REM
REM
REM   Once your program is correct you will want to end this batch file
REM   and LINK your program using the LIBRARY diskette.
REM
REM   You can end this program at any time by pressing CTRL BREAK.
REM
REM   You will need to copy EDLIN.COM from the IBM PERSONAL
REM   COMPUTER DOS diskette onto your SCRATCH diskette for
REM   this batch file to work.  Be sure B: is the default drive.
REM
PAUSE...EXIT NOW IF YOU NEED TO START AGAIN
REM
REM   OK, let's start...
REM
EDLIN %1.COB
REM
REM   The following command will give you an object file and 
REM   listing file under the name you specified.
REM
A:COBOL %1,,;
REM
REM   The next command will call another batch file which is identical
REM   to this one but without the comments (REM).  You will first be
REM   put into EDLIN.  Once you are familiar with the instructions
REM   in RUNEC.BAT you may run the uncommented batch file directly by
REM   typing: RUNED FILENAME.
REM
RUNED %1
