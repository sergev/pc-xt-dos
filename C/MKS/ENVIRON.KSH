# MKS Toolkit sample environ.ksh -- Modify to taste.
# The file named by expansion of environment variable ENV
# is executed at startup of sh.exe.
# When sh.exe is started with the "-L" option,
# this file is executed after the profile.ksh.
# This is the place to perform initialization internal to the shell.

# Do not log any function definitions in the history file.
# This is required if the etc/newhist.ksh routine is used.
set -o nolog

# Aliases to mimic command.com and access its built-in commands --
# you may want some of these during the weaning period.
alias a:="cd a:" b:="cd b:" c:="cd c:" d:="cd d:" e:="cd e:"
alias md=mkdir rd=rmdir
 : ${COMSPEC:=${ROOTDIR-c:}/command.com}
if	[ `uname -v` -le 3 ]	# DOS version 3 or less
then	SWITCHAR=`switch`	# honours switchar setting;
else	SWITCHAR=/		# later versions do not.
fi
ctty() { exec > $1 2>&1 <&1; }	# change console
alias copy="'${COMSPEC}' ${SWITCHAR}c copy"
alias dir="'${COMSPEC}' ${SWITCHAR}c dir"
alias erase="'${COMSPEC}' ${SWITCHAR}c erase"
alias path="'${COMSPEC}' ${SWITCHAR}c path"
alias ren="'${COMSPEC}' ${SWITCHAR}c ren"
alias type="'${COMSPEC}' ${SWITCHAR}c type"
alias ver="'${COMSPEC}' ${SWITCHAR}c ver"
alias verify="'${COMSPEC}' ${SWITCHAR}c verify"
alias vol="'${COMSPEC}' ${SWITCHAR}c vol"

# If you will be making a habit of stopping vi, or otherwise using
# Terminate-Stay-Resident programs that are not installed from
# /etc/inittab, then this ulimit command avoids memory conflict
# in sub-shells as well as the login shell.  Remove 1st `# ':
# ulimit -d 48	# parameter can be adjusted downwards

# Here are some assorted prompts. Choose one by removing all others.
PS1="$ "			# a simple default prompt
# PS1="!% "			# prompt with history number
# typeset -u PS1='$PWD>'	# equivalent to command prompt $p$g
# typeset -u PS1='${PWD%%:*}>'	# equivalent to command prompt $n$g
# It might be instructive to figure out how this next prompt works.
# typeset -Z2 H M S; let SECONDS=`date '+(%H*60+%M)*60+%S'`
# PS1='${__[(H=SECONDS/3600%24)==(M=SECONDS/60%60)==(S=SECONDS%60)]-$H:$M:$S}>'
