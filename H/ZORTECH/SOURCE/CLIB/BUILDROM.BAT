rem _ buildrom.bat   Wed May  4 1988   Modified by: Walter Bright */
rem Build all 4 rom libraries and startup files.

del int.obj

make clean
make MODEL=M rom
if errorlevel 1 goto exit

make clean
make MODEL=C rom
if errorlevel 1 goto exit

make clean
make MODEL=L rom
if errorlevel 1 goto exit

make clean
make rom
if errorlevel 1 goto exit

rem SUCCESS

:exit
rem 
