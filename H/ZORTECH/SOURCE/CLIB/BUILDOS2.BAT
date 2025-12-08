rem _ buildos2.bat   Sun Feb 21 1988   Modified by: Walter Bright */
rem Build all 4 libraries and startup files.

rem Make libraries for OS/2

set CFLAGS=-D__OS2__ -D_MT

del *.obj
del zl?.cmd
make MODEL=M OS=__OS2__
if errorlevel 1 goto exit
zorlib zlm +zimp.lib;

make clean
make MODEL=C OS=__OS2__
if errorlevel 1 goto exit
zorlib zlc +zimp.lib;

make clean
make MODEL=L OS=__OS2__
if errorlevel 1 goto exit
zorlib zll +zimp.lib;

make clean
del int.obj
make MODEL=S OS=__OS2__
if errorlevel 1 goto exit
zorlib zls +zimp.lib;

make clean
echo SUCCESS
:exit
rem 
set CFLAGS=-DMSDOS
