echo off
cls
echo .
echo        == INSTALL.BAT  (For 3.5 Inch SuperDisk) ==
echo    This program copies all the drivers from this diskette
echo    (the "source-drive") to a new subdirectory named "WDPLUS"
echo    that it creates on a hard disk (or floppy disk) in 
echo    another drive (the "destination-drive").
echo .
echo    The correct format for executing this installation program is:
echo .
echo         INSTALL   source-drive  destination-drive  [Enter]
echo .
echo    The source-drive that you selected is  %1
echo    The destination-drive that you selected is  %2
echo .
echo    If you have not entered the command correctly, terminate this
echo    program now by pressing CTRL+BREAK.
echo .
pause
if not exist %1\JUMPERS.DOC goto exit
echo    Making directories ...
if not exist %2\wdplus\readme.doc md %2\wdplus
if not exist %2\wdplus\netwar20\release.doc md %2\wdplus\netwar20
if not exist %2\wdplus\netwar21\release.doc md %2\wdplus\netwar21
if not exist %2\wdplus\decnet\release.doc md %2\wdplus\decnet
if not exist %2\wdplus\3plus\release.doc md %2\wdplus\3plus
if not exist %2\wdplus\lanman\release.doc md %2\wdplus\lanman
if not exist %2\wdplus\diagnose\release.doc md %2\wdplus\diagnose
if not exist %2\wdplus\netbios\release.doc md %2\wdplus\netbios
if not exist %2\wdplus\nadd\release.doc md %2\wdplus\nadd
if not exist %2\wdplus\nwpatch1\release.doc md %2\wdplus\nwpatch1
if not exist %2\wdplus\setup\release.doc md %2\wdplus\setup
echo    Copying files ...
copy %1\*.doc %2\wdplus
copy %1\wd*.* %2\wdplus\netwar21
copy %1\netwar20\*.* %2\wdplus\netwar20
copy %1\netwar21\*.* %2\wdplus\netwar21
copy %1\decnet\*.* %2\wdplus\decnet
copy %1\3plus\*.* %2\wdplus\3plus
copy %1\lanman\*.* %2\wdplus\lanman
copy %1\diagnose\*.* %2\wdplus\diagnose
copy %1\netbios\*.* %2\wdplus\netbios
copy %1\nadd\*.* %2\wdplus\nadd
copy %1\nwpatch1\*.* %2\wdplus\nwpatch1
copy %1\setup\*.* %2\wdplus\setup
copy %1\*.adf %2\wdplus
echo .
echo End of INSTALL.BAT 
