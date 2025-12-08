rem _ buildlib.cmd   Tue Dec 19 1989 */
rem Build all 4 libraries and startup files.

del int.obj
set cflags=-D_MT
make clean
make OS=__OS2__ A=^^^& MODEL=M
if errorlevel 1 goto exit

make clean
make OS=__OS2__ A=^^^& MODEL=C
if errorlevel 1 goto exit

make clean
make OS=__OS2__ A=^^^& MODEL=L
if errorlevel 1 goto exit

make clean
make OS=__OS2__ A=^^^& MODEL=S
if errorlevel 1 goto exit
make clean

echo SUCCESS
:exit
rem 
set cflags=
