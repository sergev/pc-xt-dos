rem DOS MAKE Batch File for C++ Tools Libraries
rem Copyright (C) Zortech Ltd.  1990
rem All Rights Reserved
rem Written by NGE & MDL (1/15/91)
rem Modified by DJB 26/6/91

echo off
make clean
make -ftlsasm.mak MODEL=L
if ERRORLEVEL 1 goto nextl
make -ftlsasm.mak MODEL=C
if ERRORLEVEL 1 goto nextc
make -ftlsasm.mak MODEL=M
if ERRORLEVEL 1 goto nextm
make -ftlsasm.mak MODEL=S
if ERRORLEVEL 1 goto nexts
make -ftlsasm.mak MODEL=V
if ERRORLEVEL 1 goto nextv
make -ftlsasm.mak MODEL=Z
if ERRORLEVEL 1 goto nextz
make -ftlsasmp.mak MODEL=X
if ERRORLEVEL 1 goto nextx
make -ftlsasmp.mak MODEL=P
if ERRORLEVEL 1 goto nogo
:nextl
make MODEL=L
if ERRORLEVEL 1 goto nogo
make clean
:nextc
make MODEL=C
if ERRORLEVEL 1 goto nogo
make clean
:nextm
make MODEL=M
if ERRORLEVEL 1 goto nogo
make clean
:nexts
make MODEL=S
if ERRORLEVEL 1 goto nogo
make clean
:nextv
make MODEL=V
if ERRORLEVEL 1 goto nogo
make clean
:nextz
make MODEL=Z
if ERRORLEVEL 1 goto nogo
make clean
:nextx
make -fmakefile.386 MODEL=X
if ERRORLEVEL 1 goto nogo
make clean
make -fmakefile.386 MODEL=P
if ERRORLEVEL 1 goto nogo
make clean
copy *.lib ..\..\lib
del cppt?.lib
echo Build Completed
goto ends
:nogo
echo Build Failed
:ends
