# MKS Toolkit sample profile.ksh -- Modify to taste.
# This file is executed second by "sh -L", after /etc/profile.ksh;
# found in the HOME directory (defined by login from /etc/passwd),
# or in the root directory if HOME is not defined.
# It should contain individual initialization.
#
# There is also a sample /etc/profile.ksh which you may inspect.

# HOME is normally defined by login.  Define it here if it is not
# defined already.
export HOME; : ${HOME:=$ROOTDIR}

# A default PATH should be defined in /etc/profile.ksh.
# You can add to the PATH to suit a particular login.
#export PATH="$PATH;${HOME%/}/bin"	# Note quotes, since PATH contains `;'

# Set up environment file, to be processed by all shells at startup.
export ENVIRON=${HOME%/}/environ.ksh

# The following is the standard idiom for processing ENVIRON
# in *interactive* shells only.
# export ENV='${ENVIRON[(_$-=1)+(_=0)-(_$-!=_${-%%*i*})]}'

# The following is a simple definition that will process ENVIRON
# in *all* shells.
export ENV=$ENVIRON

# LOGNAME is used by uucp and mail.
export LOGNAME=${LOGNAME:-mks}

# If LOGNAME is com then we really want to run
# $COMSPEC (after setting up the environment).
if [ $LOGNAME = 'com' ] ; then
	. $ROOTDIR/etc/dospath
	export PROMPT='$p$g'
	unset SHELL
	exec $COMSPEC /e:1024
fi
