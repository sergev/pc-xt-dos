ifdef(`prog',, `dnl--	Evaluate args; whitespace between.
dnl-
dnl- Macro to simply evaluate its arguments in order (much like lisp).
dnl- This is intended to allow better readability of macros.
dnl-
define(`prog',``$1'')dnl		simply yeild arg, quoted
include(`iterize.m4')iterize(`prog')dnl	redefine for arbitrary number of args
')dnl close ifdef(prog, ...
