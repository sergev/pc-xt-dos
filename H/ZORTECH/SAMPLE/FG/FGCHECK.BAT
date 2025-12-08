@echo off

if "%1" == "" goto error

set fg_display=
echo.
echo Default response.
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=CGAMEDRES
echo.
echo %FG_DISPLAY%
pause

%1
set fg_display=CGAHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=EGALOWRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=EGACOLOR
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=EGAMONO
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=EGAECD
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VGA11
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VGA12
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VGA13
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=ORCHIDPROHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=TSENGHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=PARADISEHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=TRIDENTHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=ATI61
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=ATI62
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=ATI63
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VEGAVGAHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=DFIHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA6A
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=EVGAHIRES
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=TOSHIBA
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA0
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA1
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA2
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA3
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA4
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA5
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA6
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=VESA7
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=HERCHALF
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=HERCFULL
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=8514A
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=TIGA
echo.
echo %FG_DISPLAY%
pause
%1 %2 %3 %4 %5 %6 %7 %8 %9

set fg_display=
goto end

:error
echo Error.  Need a program to test.
echo 

:end
echo.
echo Done.

