echo off
cls
echo    ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออป
echo    บ     WATCOM Editor Software Installation Procedure     บ
echo    บ                                                       บ
echo    บ     Copyright by WATCOM Systems Inc. 1987             บ
echo    ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
echo ÿ
rem Lines which contain only "echo" really don't.
rem They are followed by CHR$(255).
if ?.==%1. goto EXPLPARM
if .==%1. goto EXPLPARM
if .==%2. goto EXPLOUT
echo If you have not already done so, you should read the "Read Me First" document
echo provided with the software in printed form and on a diskette as the file
echo "README.1ST".  This installation procedure will copy all files, including
echo those that are not absolutely required to run the software, from the diskette
echo to the specified destination.  If, because of space limitations, you wish to
echo copy only a subset of these files then you should perform a manual software
echo installation using the guidelines described in the "Read Me First" document.
echo Approximately 139K bytes of disk space is required for the software on this
echo diskette.
echo ÿ
%1
cd \
if not exist WPROMPT.COM goto INPERR
WPROMPT c,%2\
if not errorlevel 1 goto CONTINUE
WPROMPT c,%2.
if errorlevel 1 goto OUTERR
:CONTINUE
WPROMPT p,Do you want to continue (Y/n)? ,y,n
if errorlevel 2 goto LEAVE
if not exist AUTHORIZ.COM goto NOAUTH
echo The diskette in drive %1 should be a copy of the original diskette and
echo must not be write-protected.
WPROMPT p,Do you want to continue (Y/n)? ,y,n
if errorlevel 2 goto LEAVE
AUTHORIZ
if errorlevel 1 goto LEAVE
:NOAUTH
if not exist WEDIT.EXE goto DONE
echo ÿ
echo If you wish, INSTALL can check for files in "%2" which will be overwritten
echo by files in "%1".  For your protection, this step is recommended when
echo installing the software onto a disk or diskette which already contains
echo files.  This step is unnecessary when the output location does not contain
echo any files and should be skipped since it is quite time-consuming.
WPROMPT p,Do you want INSTALL to check for existing files (Y/n)? ,y,n
if errorlevel 2 goto NOCHECK
echo Checking for files which already exist in "%2".  Please wait...
for %%i in (*.*) do if exist %2%%i echo %2%%i already exists and will be replaced.
:NOCHECK
echo ÿ
echo The WATCOM software in drive "%1" will be copied to "%2*.*".
WPROMPT p,Do you want to continue (Y/n)? ,y,n
if errorlevel 2 goto LEAVE
echo Copying %1WEDIT.EXE to %2*.*
copy %1WEDIT.EXE %2*.*
echo Copying %1WEDIT.HLP to %2*.*
copy %1WEDIT.HLP %2*.*
echo Copying %1PLAY.FTR to %2*.*
copy %1PLAY.FTR %2*.*
echo Copying %1PLAYJR.FTR to %2*.*
copy %1PLAYJR.FTR %2*.*
echo Copying %1CONFIG.COM to %2*.*
copy %1CONFIG.COM %2*.*
echo Copying %1WTALK.COM to %2*.*
copy %1WTALK.COM %2*.*
echo Copying %1CSET.COM to %2*.*
copy %1CSET.COM %2*.*
echo Copying %1COMPRESS.EXE to %2*.*
copy %1COMPRESS.EXE %2*.*
echo Copying %1*.SNG to %2*.*
copy %1*.SNG %2*.*
:DONE
echo ÿ
echo The installation has completed.  If there are any other software diskettes
echo which have not been installed yet, be sure to do them.  Once done you should
echo store the original diskette(s) and your backup(s) in a safe place.  If you
echo wish, you may configure the software as described in the User's Guide.
goto LEAVE
:INPERR
echo The specified input device does not contain the software to be installed.
goto LEAVE
:OUTERR
echo The specified output device or directory does not exist.
goto LEAVE
:EXPLOUT
echo The second parameter specifying the output location is missing.
goto LEAVE
:EXPLPARM
echo The install procedure requires two parameters.  The first parameter designates
echo the diskette drive containing the WATCOM source diskette.  The second parameter
echo designates the location where the WATCOM software should be placed.  When
echo specifying a subdirectory, you must include the trailing slash.
echo ÿ
echo  Examples: a:install  a:  b:\      Copy to root directory of disk B:
echo            a:install  a:  c:\cmds\ Copy to "CMDS" directory of disk C:
echo            a:install  a:  c:       Copy to current directory of disk C:
echo ÿ
echo Before commencing the installation process, WATCOM recommends that you make a
echo copy of the diskette using the DOS DISKCOPY program and that you use this copy
echo for the installation procedure.  If you are installing the software onto a
echo diskette, make sure that the diskette has been formatted.  Approximately 139K
echo bytes of disk space is required for the software.
:LEAVE
