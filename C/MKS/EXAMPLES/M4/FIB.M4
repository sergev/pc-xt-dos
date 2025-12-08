# macro to compute Fibonacci sequence
# fib(1st,2nd)
# 1st and 2nd arguments are 1st and 2nd elements of the sequence
# both default to 1
include(`prog.m4')dnl
include(`while.m4')dnl
define(`fib', `prog(
	pushdef(`x', ifelse($1,,1,$1)),
	pushdef(`y', ifelse($2,,1,$2)),
	while(`x>0', `prog(
		x	,
		pushdef(`z', eval(x+y)),
		define(`x', y),
		define(`y', z),
		popdef(`z'),
	)'),
	popdef(`x'),
	popdef(`y'),
)')dnl
