echo off
echo Please use the makefile in this directory.
echo .   .   .   .   .   .   .   .   .   .   .

set cflags=-D_MT
make -fmakefile.os2 && goto good
echo Build failure.
goto end
:good
echo Operation complete.
:end
set cflags=
