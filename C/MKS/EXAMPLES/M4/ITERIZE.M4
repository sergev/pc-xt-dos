ifdef(`iterize',, `dnl--	iterate macro definition over list of args
dnl- 
dnl- Cause macro defined on single arg to be iterated over entire arg list.
dnl- The changequote is required in order to (re)define the named macro with
dnl- a quoted 3rd argument passed to ifelse().  Since a literal $@ needs to
dnl- be embedded in that arg, the characters need to be separated at the time
dnl- the define() in iterize() is executed (so that they are not expanded as
dnl- the list of arguments to iterize()).  This is accomplished by inserting
dnl- a null string between the $ and @.  However, if this is done with the
dnl- same quote characters as surround the whole 3rd arg, then the null
dnl- string does not get stripped until the 3rd arg is expanded by the
dnl- execution of ifelse().  This is too late: then shift() would get the
dnl- literal $@, rather than the args to the original macro being iterized.
dnl- The solution is to change quotes during the execution of iterize(), so
dnl- that in calling define() on the named macro, the 3rd arg to ifelse() can
dnl- be passed in two pieces (separated between the $ and @), each piece
dnl- containing an unbalanced original quote.
dnl- 
define(`iterize',
	`define(`$1',
		defn(`$1')changequote(")"`'ifelse(eval($""#>1),1,`$1(shift($""@))')"changequote)')dnl
')dnl close ifdef(iterize, ...
