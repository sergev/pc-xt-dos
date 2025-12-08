#
# dossify command by wrapping in function to translate '/' to '\'
# and '-' to '/'.
#
# Usage: . dossify dos-command [dos-command]*
#
case $# in
0)	print -r - "Usage: . dossify command ..."
	;;
*)	case $0 in
	*dossify*) print -r - "Usage: . dossify command ..."
		;;
	esac
esac

for f; do
	eval function $f '	{
		integer i=0
		typeset a arg
		for a in '$f' "$@"; do
			case "$a" in
			-?*)	a="/${a#-}"
				;;
			*/*)	while :; do case "$a" in
					*/*)	a="${a%%/*}\\${a#*/}";;
					*)	break;;
					esac
				done
				;;
			esac
			arg[i]="$a"; i=i+1
		done
		print -r - "${arg[@]}"
		command "${arg[@]}"		# preserve arg spacing
	}'
done
