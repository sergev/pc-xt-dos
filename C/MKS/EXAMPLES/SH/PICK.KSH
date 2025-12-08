#
# Utility to interactively filter stream.
# Prompts on standard error with each line of standard input;
# copies line to standard output depending on response:
#	y	copy this line
#	n	do not copy this line
#	Y	copy this line and all following of which this is a prefix
#	N	do not copy this or any following of which this is a prefix
#	a	copy this line and all following lines
#	q	do not copy this or any following lines; quit
#
# The handling of responses Y and N are intended to be useful for treating
# entire directories in the output of find.
#
# A command line option of -y means treat an empty response
# as if it were y.  Likewise for all the other responses.
#

#Parse options
case $# in
0)	;;
1)	case $1 in
	-[anNyYq]) DefAns=${1#-};;
	*)	print >&2 - "Usage: $0 [-[anNyYq]]"
		exit 2;;
	esac;;
*)	print >&2 - "Usage: $0 [-[anNyYq]]"
	exit 2;;
esac

IFS=	#Do not strip leading whitespace

while	read -r line
do	if	[ -n "$match" ]
	then eval "case \"$line\" in
		$match*) case	\"$ans\" in
			[aY])	print -r - \"$line\";;
			esac
			continue;;
		esac"
	fi
	read <&2 -r ans?"$line? "	# prompt on stderr
	case	"${ans:=${DefAns}}" in
	n)	match=;;
	N)	match="$line";;
	y)	match=;		print -r - "$line";;
	Y)	match="$line";	print -r - "$line";;
	a)	match='*';	print -r - "$line";;
	q)	exit 0;;
	esac
done
