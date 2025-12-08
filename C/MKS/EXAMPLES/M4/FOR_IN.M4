ifdef(`for_in',, `dnl for_in(`control', `body with control', `instance', ...)
define(`for_in',
`ifelse($#, 0, ``$0'',
	$#, 1, ,
	$#, 2, ,
	$#, 3,	`pushdef(`$1',``$3'')$2`'popdef(`$1')',
		`pushdef(`$1',``$3'')$2`'popdef(`$1')dnl
$0(`$1', `$2', shift(shift(shift($@))))')')dnl
')dnl close ifdef(for_in, ...
