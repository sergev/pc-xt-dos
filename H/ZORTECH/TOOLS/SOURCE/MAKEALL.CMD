rem OS/2 MAKE Batch File for C++ Tools Libraries
rem Copyright (C) Zortech Ltd.  1990
rem All Rights Reserved
rem Written by NGE & MDL (1/15/91)
rem Modified by DJB 26/6/91

echo off
set cflags=-D_MT
make clean
make -ftlsasm.mak MODEL=L OS=__OS2__ || goto nextl
make -ftlsasm.mak MODEL=C OS=__OS2__ || goto nextc
make -ftlsasm.mak MODEL=M OS=__OS2__ || goto nextm
make -ftlsasm.mak MODEL=S OS=__OS2__ || goto nogo
:nextl
make A=^^^& MODEL=L OS=__OS2__ || goto nogo
make clean
:nextc
make A=^^^& MODEL=C OS=__OS2__ || goto nogo
make clean
:nextm
make A=^^^& MODEL=M OS=__OS2__ || goto nogo
make clean
make A=^^^& MODEL=S OS=__OS2__ || goto nogo
make clean
copy *.lib ..\..\plib
del cppt?.lib
echo Build Completed
goto ends
:nogo
echo Build Failed
:ends
set cflags=
