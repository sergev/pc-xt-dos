#!/u/rd/bin/sh
# diff3 [-bhHex3EX] file1 file2 file3
#
# This is a COMPLETE implementation of diff3
# using sh and awk
#
set -- `getopt bhHex3EXTV "$@"`
DFLAGS=""
FLAGS=""
TMPSAVE=0
for	arg
do	case $arg in
	-b)	DFLAGS="$DFLAGS $arg"
		FLAGS="$FLAGS bcmpflag=1"; shift;;
	-[hH])	DFLAGS="$DFLAGS $arg"; shift;;
	-e)	FLAGS="$FLAGS edXflag=1 ed3flag=1"; shift;;
	-x)	FLAGS="$FLAGS edXflag=1"; shift;;
	-3)	FLAGS="$FLAGS ed3flag=1"; shift;;
	-E)	FLAGS="$FLAGS edXflag=1 ed3flag=1 ed2flag=1"; shift;;
	-X)	FLAGS="$FLAGS edXflag=1 ed2flag=1"; shift;;
	-T)	TMPSAVE=1; shift;;
	-V)	set -x; shift;;
	--)	shift; break;;
	esac
done

case $# in
5)	MARKS="mark1=$4 mark3=$5";;
4)	MARKS="mark1=$4 mark3=$3";;
3)	MARKS="mark1=$1 mark3=$3";;
*)	echo "Usage: $0 [-ex3EX] file2 file2 file3 [mark1 [mark3]]"; exit 1;;
esac

if ((TMPSAVE))
then	TMPDIR=.
else
	TMPDIR=${TMPDIR-/tmp}
	trap "rm -f $TMPDIR/d3$$.1 $TMPDIR/d3$$.2" 0
fi
diff $DFLAGS $1 $3 >$TMPDIR/d3$$.1
if [ $? -gt 1 ]
then
	exit 2
fi
diff $DFLAGS $2 $3 >$TMPDIR/d3$$.2
if [ $? -gt 1 ]
then
	exit 2
fi
awk -f `whence diff3.awk` $FLAGS $MARKS $TMPDIR/d3$$.1 $TMPDIR/d3$$.2
