ifdef(`_include',, `dnl--	Redefine include to search M4INCPATH
include(`import.m4')dnl
include(`prog.m4')dnl
include(`for.m4')dnl
ifdef(`M4INCPATH',, `import(`M4INCPATH')')dnl
ifdef(`COMSPEC',, `import(`COMSPEC')')dnl
define(`PATHSEP', `ifelse(defn(`COMSPEC'), `', `:', `;')')dnl	for OS
define(`_include', defn(`include'))dnl		rename builtin include
define(`include', `dnl
ifelse(defn(`M4INCPATH'), `',
		`_include(`$1')',
	index(`$1', `/'), 0,
		`_include(`$1')',
	index(`$1', `:')PATHSEP, 1;,
		`_include(`$1')',
	`prog(
		pushdef(`dirs'),
		pushdef(`i'),
		pushdef(`dir'),
		pushdef(`odir'),
		pushdef(`path'),
		for(	`define(`dirs', defn(`M4INCPATH'))',
			`prog(	define(`i', index(defn(`dirs'), PATHSEP)),
				ifelse(	i,-1, `prog(
						define(`dir', defn(`dirs')),
						define(`dirs', `'),
					)', `prog(
						define(`dir',
							substr(defn(`dirs'),
								0, i)),
						define(`dirs',
							substr(defn(`dirs'),
								eval(i+1))),
					)'),
				define(`path', ifelse(defn(`dir'), , `',
							`defn(`dir')/')`$1'),
				ifelse(len(defn(`odir', `dir', `dirs')), 0,
						`errprint(`include($1): no such file
')0',
				`ifelse(syscmd(`test -r 'defn(`path'))sysval(), 0,
						`_include(defn(`path'))0',
						`1')'),
			)',
			`define(`odir', defn(`dir'))',
		),
		popdef(`path'),
		popdef(`odir'),
		popdef(`dir'),
		popdef(`i'),
		popdef(`dirs'),
	)')dnl
')dnl close define(include, ...
')dnl close ifdef(include, ...
