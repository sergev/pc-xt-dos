echo off
echo Please use the makefile in this directory.
echo .   .   .   .   .   .   .   .   .   .   .

make -fmakefile.dos
if ERRORLEVEL 1 goto nogo
echo Operation complete.
goto end
:nogo
echo Build failure.
:end
