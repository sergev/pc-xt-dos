

if exist %1.sym goto symok
rem >> ERROR: symbol file %1.sym does not exist
goto exit

:symok
if exist %1.prj goto linkprj
if exist %1.obj goto linkobj
rem >> ERROR: Objectfile %1.obj (or Project file %1.prj) does not exist
goto exit

:linkobj
link %3init %1 + %1.SYM,%2%1,,%3PROLOG
if errorlevel 1 goto exit
goto run

:linkprj
link %3init @%1.prj %1.SYM,%2%1,,%3PROLOG
if errorlevel 1 goto exit

:run
Rem Press Return to execute the program, ^C to Abort
pause
%2%1

:exit
pause
