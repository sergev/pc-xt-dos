!topic: RECOVERY
^BRECOVERY

Recover deleted files and
repair damaged ones;
restore formatted disks; and
protect disks against data loss.

!command: Disk Doctor
!dos-cmd: NDD
^BNorton Disk Doctor
NDD [drive:][drive:]... [SWITCHES]

Automatically detect and correct
physical and logical disk errors.

^BCOMMONLY USED SWITCHES
/QUICK     Just test System Area
/COMPLETE  Test System and Data Areas
/R:^Bfile^N    Write a report to 'file'
/RA:^Bfile^N   Append a report to 'file'
/X:^Bd^N       Exclude drive from testing

!command: Disk Editor
!dos-cmd: DISKEDIT
^BDisk Editor
DISKEDIT [drive:][path][file] [SWITCHES]

View and edit the entire contents of a
diskette or hard disk, including disks
that DOS cannot recognize or access.
Repair or rescue damaged files or their
data.

^BCOMMONLY USED SWITCHES
/M   Maintenance mode 컴 bypass DOS and
     examine disk directly

!command: Disk Tools
!dos-cmd: DISKTOOL
^BDisk Tools
DISKTOOL

A set of six utilities to make disks
bootable, recover from the DOS RECOVER
command, revive defective diskettes,
mark clusters, and create or restore
data rescue diskettes.

!command: Erase Protect
!dos-cmd: EP
^BErase Protect
EP [SWITCHES]

Protect deleted files from being
overwritten immediately 컴 helps
ensure future recovery of deleted data.

^BCOMMONLY USED SWITCHES
/STATUS     Display Erase Protect status
/ON         Enable Erase Protect
/OFF        Unload Erase Protect
/UNINSTALL  Same as /OFF

!command: File Fix
!dos-cmd: FILEFIX
^BFile Fix
FILEFIX [filename]

Diagnose and repair damaged
Lotus 1-2-3, Symphony, and
dBASE files. "UnZap" dBASE files.

!command: Image
!dos-cmd: IMAGE
^BImage
IMAGE [drive:][drive:]... [/NOBACK]

Ensure that accidentally formatted
disks can be recovered 컴 takes a
"snapshot" record of a disk's vital
bookkeeping information.

^BSWITCH
/NOBACK  Overwrite current IMAGE.DAT
         file (IMAGE.BAK backup file
         is not created)

!command: UnErase
!dos-cmd: UNERASE
^BUnErase
UNERASE [filename]

Search for and recover erased
(deleted) files.

Automatically recover intact files
컴 manual mode allows recovery of
data from files that have been
partially overwritten or fragmented.

!command: UnFormat
!dos-cmd: UNFORMAT
^BUnFormat
UNFORMAT [drive:]

Undo an accidental formatting of a
hard disk 컴 the data is accessible
again.

UnFormat lets you recover from an
accidental format, even if no prior
system information was saved. A
near-perfect recovery can be achieved
if a "snapshot" of the system
information was saved with IMAGE.

!topic: SPEED
^BSPEED

Enhance your computer's performance
and protect your data on an everyday
basis.

!command: Calibrate
!dos-cmd: CALIBRAT
^BCalibrate
CALIBRAT [drive:] [SWITCHES]

Optimize the speed and reliability of
data transfer to and from your hard
disk.

^BCOMMONLY USED SWITCHES
/BATCH    Skip all prompts, exit to DOS
          when done
/NOSEEK   Do not perform seek tests
/NOFORMAT Do pattern testing only, do
          not format
/X:^Bdrives^N Don't test listed drives

!command: Norton Cache
!dos-cmd: NCACHE /STATUS
^BNorton Cache

Improve performance by using some of
of your computer's RAM as a holding
area for data read from your disks
컴 frequently-needed data can be
accessed faster.  Can also be used
to buffer data that is to be written
to your disk.

See the Users Guide for complete
information on Norton Cache.

!command: Speed Disk
!dos-cmd: SPEEDISK
^BSpeed Disk
SPEEDISK [drive:] [SWITCHES]

Optimize your hard disk by
reorganizing the layout of all files
and directories on the disk; also
consolidate free space and
unfragment files.

^BCOMMONLY USED SWITCHES
/B  Reboot after optimization
/F  Optimize complete disk
/Q  Compress free space only
/U  Unfragment files only

!topic: SECURITY
^BSECURITY

Secure data from unauthorized use
or examination and protect it
from viral infection.

!command: Disk Monitor
!dos-cmd: DISKMON
^BDisk Monitor
DISKMON [SWITCHES]

Prevent data from being written to disk
without your approval; display name of
drive currently being accessed in the
upper-right corner of screen;
park hard disk heads.
^BCOMMONLY USED SWITCHES
/STATUS    On-screen status summary
/PROTECT+  Turn protect features on
/LIGHT+    Turn DiskLight feature on
/PARK      Park disk drive heads
/UNINSTALL Remove DISKMON from memory

!command: Diskreet
!dos-cmd: DISKREET
^BDiskreet
DISKREET [SWITCHES]

Encrypt and password-protect files.
Create a password-protected "virtual"
disk drive on which all files are
encrypted.

^BCOMMONLY USED SWITCHES
/ENCRYPT:^Bfilename^N Encrypt specified file
/DECRYPT:^Bfilename^N Decrypt specified file
/PASSWORD:^Bxxxxxx^N  Set password to xxxxxx

!command: WipeInfo
!dos-cmd: WIPEINFO
^BWipeInfo
WIPEINFO [drive:] [SWITCHES]
WIPEINFO [filename] [SWITCHES]

Totally obliterate erased (deleted)
data so that it cannot be recovered
by any means 컴 single files or
entire disks.

^BCOMMONLY USED SWITCHES
/E  Wipe erased/unused space on disk
/N  Delete files DOS-style without
    wiping
/K  Wipe slack space at end of file

!topic: TOOLS
^BTOOLS

Programs to make your computing faster
and easier.

Control your computer's display and
keyboard, set timers, find files,
change directories quickly, format
diskettes, find out information about
your computer system, and more.

!command: Batch Enhancer
!dos-cmd: BE ?
^BBatch Enhancer
BE subcommand(s)

Automate procedures by creating batch
files that can be controlled in real
time.

Draw demonstration and sign-on screens.





^B컴^N Use in batch files.

!command: Configuration
!dos-cmd: NUCONFIG
^BNorton Utilities Configuration
NUCONFIG

Re-configure setup options for the
Norton Utilities; install or remove
device drivers and programs run at
system startup.

!command: Control Center
!dos-cmd: NCC
^BNorton Control Center
NCC [filename] [SWITCHES]

Control your PC's hardware settings.
Set the cursor size, change DOS
colors, set palette colors, select
video mode, change keyboard settings,
set mouse speed, and more.

^BCOMMONLY USED SWITCHES
/SET   Set all options saved in ^Bfilename
/CO80  Set display to 25x80 color
/43    Set display to 43 lines (EGA)
/START:^Bn^N  Start watch number 'n'

!command: Directory Sort
!dos-cmd: DS
^BDirectory Sort
DS [path]
DS sortkeys [path] [/S]

Sort directory listings.

^BSORT KEYS
N   name          Append a dash to the
D   date          sort key to reverse
E   extension     the sort order.
S   size
T   time
^BSWITCH
/S  Sort subdirectories

!command: File Attributes
!dos-cmd: FA
^BFile Attributes
FA [filename] [SWITCHES]

Display files by attribute; change
attributes of files and directories.

^BCOMMONLY USED SWITCHES
/A    archive         Use ^B+^N or ^B-^N after
/R    read-only       switch to set or
/HID  hidden          clear attribute.
/SYS  system


^B컴 ^NFA is a command line utility.

!command: File Date
!dos-cmd: FD
^BFile Date
FD filename [SWITCHES]

Change file time and date information.

^BCOMMONLY USED SWITCHES
/D:^Bdate^N  Set date to mm-dd-yy
/T:^Btime^N  Set time to hh:mm:ss
/S       Include files in Subdirectories

FD without /D and /T will stamp the
current date and time on the file.

^B컴^N FD is a command line utility.

!command: File Find
!dos-cmd: FILEFIND
^BFile Find
FILEFIND [wildcard] [text] [SWITCHES]

Locate misplaced files by searching
for their data or names through all
available drives (including network
drives) and directories. Change file
attributes.

^BCOMMONLY USED SWITCHES
/S      Include files in subdirectories
/CS     Case-sensitive search
/CLEAR  Clear all file attributes
/NOW    Set time & date to current time

!command: File Locate
!dos-cmd: FL
^BFile Locate
FL [drive:] [filename] [SWITCHES]

Locate a file. FL by itself lists all
files on the default drive.

^BCOMMONLY USED SWITCHES
/A      Search All drives
/P      Pause after each screen
/W      List files in Wide format
/T      Search the PATH only
/F[^Bn^N]   Find the first 'n' files

^B컴^N FL is a command line utility.

!command: File Size
!dos-cmd: FS
^BFile Size
FS [pathname] [target-drive:] [SWITCHES]

Display the size of files and
directories. Find out if a group of
files will fit on another drive.

^BCOMMONLY USED SWITCHES
target-drive:    Test for enough space
/P               Pause after each screen
/S               Include Subdirectories
/T               Display Totals only

^B컴^N FS is a command line utility.

!command: Line Print
!dos-cmd: LP
^BLine Print
LP filename [output] [SWITCHES]

Print text files to printer or disk.

^BCOMMONLY USED SWITCHES
/Tn /Bn /Ln /Rn    Margins
/Wn /Hn            Page Width/Height
/TABn              Tab spacing
/N                 Line numbering ON
/HEADERn           Type of header
/80 /132           Epson/IBM font width
/PS                PostScript output
^B컴^N LP is a command line utility.

!command: Norton CD
!dos-cmd: NCD
^BNorton Change Directory
NCD [directory name] [SWITCHES]

Navigate through and manage the
directory structure on your disk,
no matter how complex. Rename, move,
copy, and delete directories.

Switch to any directory, no matter how
deep, with a few keystrokes.

^BCOMMONLY USED SWITCHES
/R  Rescan disk directory
/N  Do not write or update TREEINFO.NCD

!command: Safe Format
!dos-cmd: SFORMAT
^BSafe Format
SFORMAT [drive:] [SWITCHES]

Safely format a disk 컴 does not
overwrite data, to make recovery easier.

^BCOMMONLY USED SWITCHES
/S        Copy DOS system files
/V:^Blabel^N  Add a volume label
/F:^Bsize^N   Specify size (360, 720, etc.)
/Q        Quick format
/D        DOS format (same as DOS)

!command: System Info
!dos-cmd: SYSINFO
^BSystem Information
SYSINFO [SWITCHES]

Reports on all facets of your computer
system: hardware, memory use,
networks, available disks, TSRs and
device drivers, and more. Provides
benchmarks and performance ratings.

^BCOMMONLY USED SWITCHES
/AUTO:^Bn^N  Automatic mode, '^Bn^N' sec./screen
/DEMO    Demonstration mode
/TSR     Show only active TSRs
/N       Bypass live memory probe

!command: Text Search
!dos-cmd: TS
^BText Search
TS [pathname] [text] [SWITCHES]
TS [text] /D [SWITCHES]
TS [text] /E [SWITCHES]

Search for text in files, disks, or
erased disk space.
^BCOMMONLY USED SWITCHES
/S    Include subdirectories
/T    Non-interactive Totals mode
/C^Bn^N   Start the search at Cluster '^Bn^N'
/D    Search entire Disk
/E    Search Erased space
^B컴^N TS is a command line utility.
