# This routine will change each '/' in the PATH environment variable into '\'.
#
# Usage:
#      . $ROOTDIR/etc/dospath

while true
do
	case $PATH in
	*/*)	PATH=${PATH%%/*}\\${PATH#*/};;
	*)	break;;
	esac
done
