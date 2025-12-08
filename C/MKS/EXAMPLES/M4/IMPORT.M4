ifdef(`import',, `dnl	import macro definition from the environment
include(`prog.m4')dnl
define(`import',
`prog(
	pushdef(`tmp', maketemp(`m4XXXXX.pi')),
	syscmd(`echo "define(\`$1', \`$$1')"dnl >'tmp),
	sinclude(tmp),
	syscmd(`rm 'tmp),
	popdef(`tmp'),
)')dnl close define(import, ...
include(`iterize.m4')iterize(`import')dnl redefine for any number of args
')dnl close ifdef(import, ...
