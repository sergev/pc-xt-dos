echo off
echo              == INSTALL.BAT ==
echo    Western Digital LAN Adapter Board Drivers
echo        For Novell Advanced NetWare 2.0a
echo            Installation Procedure
echo                                                                        .
cd B:\
echo    Do you want to install the Server driver ? (y or n)
ask
if errorlevel = 3 goto shell3
:loop2
echo   Place working copy of Novell GENOS-1 diskette in drive B:
PAUSE
if not exist B:\SELOS.EXE goto loop2
md B:\WDPLUS
cd B:\WDPLUS
copy A:ACOMDRV.OBJ B:
copy A:BCOMDRV.OBJ B:
copy A:CCOMDRV.OBJ B:
copy A:DCOMDRV.OBJ B:
if not exist B:DCOMDRV.OBJ goto error
:shell3
echo    Do you want to install the Shell driver ? (y or n)
ask
if errorlevel = 3 goto exit
:loop3
echo   Place working copy of Novell GENSH-1 diskette in drive B:
PAUSE
if not exist B:\SELSH.COM goto loop3
md B:\WDPLUS
cd B:\WDPLUS
copy A:SHELLDRV.OBJ B:
if not exist b:SHELLDRV.OBJ goto error
goto exit
:error
echo    ERROR:  There is not enough free space on the Novell
echo            diskette in drive B:
echo    Please remove one or more drivers and subdirectories
echo    that you do not need from the B: disk.
echo    Then repeat this INSTALL batch file.
echo
echo    CAUTION:  Do not remove NULL directory on GENOS-1 diskette.
echo
:exit
echo    END INSTALL
cd B:\
