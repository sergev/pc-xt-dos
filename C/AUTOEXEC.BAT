@ECHO OFF
rem Suppresses command display during boot for a cleaner startup.
rem The "@" hides even this line.

PROMPT $P$G
rem Shows the current drive and directory followed by ">" (e.g., "C:\DOS>").

PATH c:\dos;c:\vc;c:\util
rem Adds directories to the search path so you can run commands like DIR
rem or FORMAT without typing the full path.
rem Expand as needed (e.g., PATH C:\DOS;C:\GAMES).

FASTOPEN C:=50
rem Caches up to 50 file locations in RAM for quicker hard drive access.
rem Adjust lower (e.g., 20) if memory is tight.
rem Omit if no hard drive or for pure floppy use (uses ~2.5KB at =50).

set TMP=c:\
rem For Microsoft Fortran compiler.

turboon
rtclock get 2025
vc
