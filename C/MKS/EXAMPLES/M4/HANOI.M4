ifdef(`hanoi',, `dnl
# hanoi takes 4 args: count, name1, name2, name3
include(`prog.m4')dnl
define(`hanoi',
	`ifelse(eval($1>0), 1,
		`prog(	hanoi(eval($1-1), $2, $4, $3),
			move ring $1 from $2 to $3
,
			hanoi(eval($1-1), $4, $3, $2),
		)')')dnl
')dnl close ifdef(hanoi, ...
