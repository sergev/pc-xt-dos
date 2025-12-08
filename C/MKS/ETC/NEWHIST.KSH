set -o nolog
function mktemp {
	typeset path="${1:-${TMPDIR:-/tmp}}"
	typeset path="${path%/}/tmp$$"
	typeset -i36 -R3 ext=36#aaa

	until
		(set -C; : >"$path.$ext") 2>|/dev/nul
	do ext=ext+1
	done

	echo "$path.$ext"
}
export HISTFILE=$(mktemp)
trap 'rm $HISTFILE; exit' EXIT

