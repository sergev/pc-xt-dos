# Script for renaming files;
# Takes a sed command as the first argument (most usefully a substitute)
# and applies it to the remaining arguments (or stdin, if no arguments)
# considered as a list of filenames.
# e.g.
#	rename 's/.txt$/.bak/' *
#	ls o*.c | rename 's/^o/old\//'

function rename_1 {
	typeset gname=`print -R "$1" | sed "$subst"`
	if	[ "$1" != "$gname" ]
	then	mv "$1" "$gname"
	fi
}

case $# in
0)	print >&2 "Usage: $0 'sed command' [file ...]"
	exit 2
	;;
1)	subst="$1"
	while read fname; do rename_1 "$fname"; done
	;;
*)	subst="$1"; shift
	for fname; do rename_1 "$fname"; done
	;;
esac
