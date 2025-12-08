rem _ buildlib.bat   Sun Feb 21 1988   Modified by: Walter Bright */
rem Build all 6 libraries and startup files.

rem Make libraries for DOS
make clean
make MODEL=M OS=MSDOS
if errorlevel 1 goto exit

make clean
make MODEL=C OS=MSDOS library swap
if errorlevel 1 goto exit

make clean
make MODEL=L OS=MSDOS library swap
if errorlevel 1 goto exit

make clean
make MODEL=V OS=MSDOS library swap
if errorlevel 1 goto exit

make clean
del int.obj
del ct.obj
make MODEL=S OS=MSDOS
if errorlevel 1 goto exit

rem Make library for DOS extender
make clean
make -fmakefile.zpm
if errorlevel 1 goto exit
make -fmakefile.zpm clean
:great
echo SUCCESS
:exit
rem 

