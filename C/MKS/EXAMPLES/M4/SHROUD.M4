ifdef(`shroud',, `dnl--		Cause macro without args to yeild own name.
dnl-
dnl- Shroud macro definition, so that it is only expanded if called with
dnl- args.  The renaming of the target is required (unlike iterize), because,
dnl- in order to check $# first, the original macro has to be passed to
dnl- ifelse() *quoted* (to delay evaluation until ifelse() has checked its
dnl- args).  The only way to pass on the arguments to this delayed evaluation
dnl- is to attach them in parentheses to a macro name (you cannot do
dnl- something like <macro-defn>(args) and get the desired effect).  And at
dnl- this point the original macro has been redefined, so you cannot use the
dnl- old name; that just results in infinite recursion.  The only option open
dnl- is to have another name for the original macro, that is used here
dnl- instead.
dnl-
define(`shroud', `ifdef(`M4_$1', , `dnl	do nothing if already shrouded
define(`M4_$1', defn(`$1'))dnl			rename argument
define(`$1', `ifelse($'`#,0,``$1'',`M4_$1'($'`@))')dnl redefine to check args
')dnl close ifdef(M4_...
')dnl close define($1, ...
include(`iterize.m4')iterize(`shroud')dnl	redefine for any number args
')dnl close define(shroud, ...
')dnl close ifdef(shroud, ...
