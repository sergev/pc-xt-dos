ifdef(`args',, `dnl			simply dump args: debugging aid
define(`args', ``$1'
')dnl					first define on single arg
include(`iterize.m4')iterize(`args')dnl	redefine for arbitrary args
')dnl	close ifdef(args, ...
