# mktemp.ksh -- return a unique temporary file name
#
# This shell function replaces the mktemp utility in previous MKS Toolkit
# releases.  Place the following line in your environ.ksh file:
#	. mktemp.ksh

function mktemp {
	typeset path="${1:-${TMPDIR:-/tmp}}"
	typeset path="${path%/}/tmp$$"
	typeset -i36 -R3 ext=36#aaa
	until (set -C; : >"$path.$ext") 2>|/dev/nul
	do ext=ext+1
	done
	echo "$path.$ext"
}
