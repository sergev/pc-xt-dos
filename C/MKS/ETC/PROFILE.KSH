# MKS Toolkit sample /etc/profile.ksh -- Modify as required.
# This file is executed first by "sh -L", and by KornShells started
# from login.  It should contain global initialization commands,
# common to all KornShell users.

# ROOTDIR is normally set by login.  It identifies the directory
# in which the Toolkit package was installed.
export ROOTDIR; : ${ROOTDIR:=c:}

# SHELL and COMSPEC are normally set by login.  SHELL specifies the
# the location of the MKS KornShell.  COMSPEC specifies the location
# of the DOS command.com interpreter.  COMSPEC is used by *many* DOS
# software packages, all of which assume that the variable points to
# command.com.
export SHELL; : ${SHELL:=$ROOTDIR/bin/sh.exe}	# not reqd if using login
export COMSPEC; : ${COMSPEC:=c:\\command.com}	# if shell=... in config.sys

# TMPDIR identifies the directory used to hold temporary files.
export TMPDIR; : ${TMPDIR:=${ROOTDIR}/tmp}

# TZ identifies the time zone in which you live.  If you do not live
# in the Eastern North American time zone, you will wish to change
# the value of TZ.  See the timezone(5) manual page.
export TZ; : ${TZ:=EST0EDT}

# You may wish to add more directories to this PATH.
export PATH=";$ROOTDIR/bin;c:/dos;c:"

# Print the "Message Of The Day".  Comment this line out, if you wish.
cat -s ${ROOTDIR}/etc/motd
