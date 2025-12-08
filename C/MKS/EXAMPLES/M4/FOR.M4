ifdef(`for',, `dnl	for(init, test, incr, stat)
include(`while.m4')dnl
define(`for', `$1`'while(`$2', `$4`'$3')')dnl
')dnl close ifdef(for, ...
