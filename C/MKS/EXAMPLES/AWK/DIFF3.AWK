# diff3.awk -- internal part of diff3 command
# COMPLETE implementation of diff3 in sh and awk languages
# Called with two files as arguments:
#	differences 1 vs. 3
#	differences 2 vs. 3
# Variables set externally (via command line):
#	bcmpflag=1	set for -b
#	edXflag=1	set for	-e -E -x -X
#	ed3flag=1		-e -E -3
#	ed2flag=1		-E -X
#	mark1="mark 1 arg"
#	mark3="mark 3 arg"

function assert(cond, diag) {	# if condition not met, exit with diagnostic
	if (!cond) {
		print diag
		exit(2)
	}
}

function range(file, d	, line, pos, cmd) { # read diff line addresses
	if ((getline line <file) <= 0) {	# EOF
		d[TL] = d[BL]+= 1000000
		d[TR] = d[BR]+= 1000000
		return -1
	}
	assert(line ~ /^[0-9]+(,[0-9]+)?[acd][0-9]+(,[0-9]+)?$/,
		file ": bad control line: " line)
	d[BL] = d[TL] = substr(line, 1, pos=match(line, "[^0-9]")) +0
	if ((cmd=substr(line, pos, 1)) == ",") {
		line = substr(line, pos+1)
		d[BL] = substr(line, 1, pos=match(line, "[^0-9]")) +0
		cmd = substr(line, pos, 1)
	}
	line = substr(line, pos+1)
	if ((pos=match(line, "[^0-9]")) == 0) {
		d[BR] = d[TR] = line +0
	} else {
		d[TR] = substr(line, 1, pos) +0
		d[BR] = substr(line, pos+1) +0
	}
	if (cmd ~ /[ac]/) {
		d[TR]--
	}
	if (cmd ~ /[cd]/) {
		d[TL]--
	}
	return 0
}

function putdiff(mark) {
	if (edit < 0) {
		print "====" mark
	} else {
		edit = edXflag&&mark=="" || ed3flag&&mark=="3"
		hilite = ed2flag&&mark==""
	}
}

function puthead(fn, a, b) {
	if (edit < 0)
		printf "%d:", fn
	if (edit < 0 || edit==1 && fn==1) {
		if (a == b) {
			print a+edAdj "a"
		} else if (a+1 == b) {
			print b+edAdj "c"
		} else {
			print a+1+edAdj "," b+edAdj "c"
		}
	}
	if (edit > 0)
		if ((edit=fn)==1)
			edAdj += hilite ? 3 : a-b
		else if (fn==3)
			edAdj += b-a
}

function dropsep(file	, line) {
	assert((getline line <file) > 0 && line ~ /^---$/,
		file ": expecting ---; got " line)
}

function putlines(lines, file, d, hand, lite, show, del	, line, got) {
	if (hilite && lite!="")
		print lite
	for (got=0; d[T hand] < d[B hand]; ++d[T hand]) {
		if (d[T hand] in lines) {
			line = lines[d[T hand]]
			if (del) delete lines[d[T hand]]
		} else {
			getline line <file
			sub(/^[<>]/, " ", line)
			++got
		}
		if (show) {
			if (edit < 0) {
				print line
			} else if (hilite && edit==1 || edit==3) {
				line = substr(line, 3)
				if (line ~ /^[.\]/)
					line = "\\" line
				print line
			}
		}
	}
	if (!show && edit==3)
		print "."
	if (got && hand==L && d[TR]!=d[BR])
		dropsep(file)
}

function savelines(lines, file, d	, a) {	# save lines from file 1 or 2
	for (a = d[TL]; a < d[BL]; ++a) {
		assert(!(a in lines), file ": rereads line " a "\n" lines[a])
		getline lines[a] <file
		sub(/^</, " ", lines[a])
	}
	if (d[TL] != d[BL] && d[TR] != d[BR])
		dropsep(file)
}

function isempty(d) {
	return (d[TL]==d[BL] && d[TR]==d[BR])
}

function deblank(line , copy) {	# crunch whitespace in line if bcmpflag set
	if (!bcmpflag)
		return (line)
	copy = line
	sub(/[ \t]+$/, "", copy)	# strip trailing whitespace
	gsub(/[ \t]+/, " ", copy)	# crunch whitespace to " "
	return (copy)
}

function samelines(lines1, file1, d1, lines2, file2, d2	, a1, a2) {
	savelines(lines1, file1, d1)
	savelines(lines2, file2, d2)
	if (d1[TR]!=d2[TR] || d1[BR]!=d2[BR] || d1[BL]-d1[TL]!=d2[BL]-d2[TL] ||
	    isempty(d1))
		return (0)
	for ((a1=d1[TL])+(a2=d2[TL]); a1 < d1[BL];)
		if (deblank(lines1[a1++]) != deblank(lines2[a2++]))
			return (0)
	return (1)
}

function readlines(lines, file, fr, to	, a) {	# save lines from file 3
	for (a = fr; a < to; ++a) {
		assert(!(a in lines), file ": reread line " a "\n" lines[a])
		getline lines[a] <file
		sub(/^>/, " ", lines[a])
	}
}

function duplines(lines, file, fr, to	, a, l) { # discard lines from file 3
	for (a = fr; a < to; ++a) {
		assert(a in lines, file ": missed line " a)
		getline l <file
		sub(/^>/, " ", l)
		assert(lines[a]==l, line ": mismatch line " a "\n" line[a] l)
	}
}

function inslines(lines, ilines, d, fr	, a) {	# prepend lines read from 3
	for (a = d[TL]; fr < d[TR];) {
		--a
		assert(!(a in lines), file ": preread line " a "\n" lines[a])
		lines[a] = ilines[--d[TR]]
	}
	d[TL] = a
}

function applines(lines, alines, d, to	, a) {	# append lines read from 3
	for (a = d[BL]; d[BR] < to; ++a) {
		assert(!(a in lines), file ": apread line " a "\n" lines[a])
		lines[a] = alines[d[BR]++]
	}
	d[BL] = a
}

function mvrange(d, nd) {
	d[TL] = nd[TL]; d[BL] = nd[TL] = nd[BL]
	d[TR] = nd[TR]; d[BR] = nd[TR] = nd[BR]
}

{exit}	# everything done in END action so VAR=x on command line gets evaluated
END	{
	T = ""; B = "B"; L = "L"; R = "R"
	TL = T L		# index of Top addr of Left file in diff
	BL = B L		# index of Bottom addr of Left file in diff
	TR = T R		# index of Top addr of Right file in diff
	BR = B R		# index of Bottom addr of Right file in diff
	ndiff = 0
	edit = -1
	file1 = file2 = ""
	for (arg=1; arg<ARGC; ++arg) {
		if (ARGV[arg] ~ /^mark[13]=/) {
		} else if (ARGV[arg] ~ /^bcmpflag=/) {
		} else if (ARGV[arg] ~ /^ed[23X]flag=/) {
			edit = 0
		} else if (file1=="") {
			file1 = ARGV[arg]
		} else if (file2=="") {
			file2 = ARGV[arg]
		}
	}
	eof1 = range(file1, d1)
	eof2 = range(file2, d2)
	while (!eof1 || !eof2) {
		if (!eof1 &&
		    (eof2 || d1[BR] < d2[TR])) { # 1<>3 precedes 2<>3
			putdiff("1")
			puthead(1, d1[TL], d1[BL])
			putlines(lines1, file1, d1, L, "", 1, 1)
			puthead(2, d2[TL]-d2[TR]+d1[TR], d2[TL]-d2[TR]+d1[BR])
			puthead(3, d1[TR], d1[BR])
			putlines(lines3, file1, d1, R, "", 1, 1)
			eof1 = range(file1, d1)
			if (ndiff==0) ndiff = 1
		} else if (!eof2 &&
			   (eof1 || d2[BR] < d1[TR])) { # 2<>3 precedes 1<>3
			putdiff("2")
			puthead(1, d1[TL]-d1[TR]+d2[TR], d1[TL]-d1[TR]+d2[BR])
			puthead(2, d2[TL], d2[BL])
			putlines(lines2, file2, d2, L, "", 1, 1)
			puthead(3, d2[TR], d2[BR])
			putlines(lines3, file2, d2, R, "", 1, 1)
			eof2 = range(file2, d2)
			if (ndiff==0) ndiff = 1
		} else if (eof1 || eof2) {
			continue
		} else if (samelines(lines1, file1, d1, lines2, file2, d2)) {
			# lines1 match lines2 and there are some lines
			putdiff("3")
			puthead(1, d1[TL], d1[BL])
			puthead(2, d2[TL], d2[BL])
			putlines(lines1, file1, d1, L, "", 1, 1)
			putlines(lines2, file2, d2, L, "", 0, 1)
			puthead(3, d1[TR], d1[BR])
			putlines(lines3, file1, d1, R, "", 1, 0)
			putlines(lines3, file2, d2, R, "", 0, 1)
			eof1 = range(file1, d1)
			eof2 = range(file2, d2)
			if (ndiff==0) ndiff = 1
		} else { # all differ
			if (d1[TR] <= d2[TR]) {
				readlines(lines3, file1, d1[TR], f3A=d2[TR])
				inslines(lines2, lines3, d2, d1[TR])
			} else { # (d2[TR] < d1[TR])
				readlines(lines3, file2, d2[TR], f3A=d1[TR])
				inslines(lines1, lines3, d1, d2[TR])
			}
			# init nd1, nd2
			while (d1[BR] != d2[BR]) {
				f3B = d1[BR] < d2[BR] ? d1[BR] : d2[BR]
				readlines(lines3, file1, f3A, f3B)
				duplines(lines3, file2, f3A, f3B)
				if (d1[BR] == f3B) {
					eof1 = range(file1, nd1)
					if ((f3A=nd1[TR])<=d2[BR]) { # overlap
						readlines(lines3, file2, f3B, f3A)
						applines(lines1, lines3, d1, f3A)
						savelines(lines1, file1, nd1)
						d1[BL] = nd1[TL] = nd1[BL]
						d1[BR] = nd1[TR] = nd1[BR]
					} else {
						readlines(lines3, file2, f3B, d2[BR])
						applines(lines1, lines3, d1, d2[BR])
					}
				} else { # (d2[BR] == f3B)
					eof2 = range(file2, nd2)
					if ((f3A=nd2[TR])<=d1[BR]) { # overlap
						readlines(lines3, file1, f3B, f3A)
						applines(lines2, lines3, d2, f3A)
						savelines(lines2, file2, nd2)
						d2[BL] = nd2[TL] = nd2[BL]
						d2[BR] = nd2[TR] = nd2[BR]
					} else {
						readlines(lines3, file1, f3B, d1[BR])
						applines(lines2, lines3, d2, d1[BR])
					}
				}
			}
			putdiff("")
			puthead(1, d1[TL], d1[BL])
			putlines(lines1, file1, d1, L, "<<<<<<< " mark1, 1, 1)
			puthead(2, d2[TL], d2[BL])
			putlines(lines2, file2, d2, L, "", 1, 1)
			puthead(3, d1[TR], d1[BR])
			putlines(lines3, file1, d1, R, "=======", 1, 0)
			putlines(lines3, file2, d2, R, ">>>>>>> " mark3, 0, 1)
			if (!eof1 && isempty(nd1))
				eof1 = range(file1, d1)
			else
				mvrange(d1, nd1)
			if (!eof2 && isempty(nd2))
				eof2 = range(file2, d2)
			else
				mvrange(d2, nd2)
			if (ed2flag) ndiff = ndiff<3 ? 3 : ndiff+1
			else if (ndiff==0) ndiff = 1
		}
	}
	exit(ndiff)
}
