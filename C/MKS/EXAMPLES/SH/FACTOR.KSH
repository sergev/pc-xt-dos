# Use BC to produce prime factors

case $# in
0)	while read num
	do
		echo "void factor($num)"
	done | bc $(whence factor.bc)
	;;
*)	for num
	do
		echo "void factor($num)"
	done | bc $(whence factor.bc)
	;;
esac
