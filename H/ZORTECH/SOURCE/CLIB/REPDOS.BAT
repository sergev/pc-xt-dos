rem Compile and replace a C module in all five libraries.
rem Use:	REPC modulename [sourcefilename]

if "" == "%2" goto module

rem MSDOS

ztc -r -c -o+space -o%1.obj %2 -DA%1
if errorlevel 1 goto err
zorlib ..\..\lib\zls -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space -mm -o%1.obj %2 -DA%1
if errorlevel 1 goto err
zorlib ..\..\lib\zlm -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space -mc -o%1.obj %2 -DA%1
if errorlevel 1 goto err
zorlib ..\..\lib\zlc -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space -ml -o%1.obj %2 -DA%1
if errorlevel 1 goto err
zorlib ..\..\lib\zll -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space -mv -o%1.obj %2 -DA%1
if errorlevel 1 goto err
zorlib ..\..\lib\zlv -+%1;
if errorlevel 1 goto err

goto success

:module

rem MSDOS

ztc -r -c -o+space %1
if errorlevel 1 goto err
zorlib ..\..\lib\zls -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space %1 -mm
if errorlevel 1 goto err
zorlib ..\..\lib\zlm -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space %1 -mc
if errorlevel 1 goto err
zorlib ..\..\lib\zlc -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space %1 -ml
if errorlevel 1 goto err
zorlib ..\..\lib\zll -+%1;
if errorlevel 1 goto err

ztc -r -c -o+space %1 -mv
if errorlevel 1 goto err
zorlib ..\..\lib\zlv -+%1;
if errorlevel 1 goto err

:success
rem SUCCESS
:err

