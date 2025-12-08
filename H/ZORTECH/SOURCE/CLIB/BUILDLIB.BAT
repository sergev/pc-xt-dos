rem _ buildlib.bat
rem Build libraries and startup files.

rem Make libraries for Windows

make clean
make MODEL=M OS=_WINDOWS XFLG=-W1 LB=zwm oldstrm.lib library winobjs
if errorlevel 1 goto exit
zorlib zwm-c-msmouse-jmp+cw+winerr+winmouse+wincvar1+wincvar2+winc+dllstart+dpmi+dpmimin+winsleep+wincio;
if errorlevel 1 goto exit
zorlib zwm-alloc+winalloc;
if errorlevel 1 goto exit
del zwm.bak

make clean
make MODEL=C OS=_WINDOWS XFLG=-W1 LB=zwc oldstrc.lib library winobjs
if errorlevel 1 goto exit
zorlib zwc-c-msmouse-jmp+cw+winerr+winmouse+wincvar1+wincvar2+winc+dllstart+dpmi+dpmimin+winsleep+wincio;
if errorlevel 1 goto exit
del zwc.bak

make clean
make MODEL=L OS=_WINDOWS XFLG=-W1 LB=zwl oldstrl.lib library winobjs
if errorlevel 1 goto exit
zorlib zwl-c-msmouse-jmp+cw+winerr+winmouse+wincvar1+wincvar2+winc+dllstart+dpmi+dpmimin+winsleep+wincio;
if errorlevel 1 goto exit
del zwl.bak

make clean
del int.obj
make MODEL=S OS=_WINDOWS XFLG=-W1 LB=zws oldstrs.lib library winobjs
if errorlevel 1 goto exit
zorlib zws-c-msmouse-jmp+cw+winerr+winmouse+wincvar1+wincvar2+winc+dllstart+dpmi+dpmimin+winsleep+wincio;
if errorlevel 1 goto exit
zorlib zws-alloc+winalloc;
if errorlevel 1 goto exit
ren int.obj iw.obj
del zws.bak

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
make MODEL=V OS=MSDOS library swap oldstrv.lib
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

