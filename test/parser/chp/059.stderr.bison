At: "059.hac":8:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(process-prototype) [3:1..59]
#STATE#	{ [3:61]
#STATE#	list<(def-body-item)>: (instance-decl) ... [4:1..9]
#STATE#	keyword: chp [5:1..3]
#STATE#	{ [5:5]
#STATE#	*[ [6:2]
#STATE#	list<(chp-statement)>: list<(chp-statement)>: (chp-receive) ... ... [7:3..7]
#STATE#	; [7:8]
#STATE#	[ [8:3]
#STATE#	(chp-guarded-cmd) [8:4..13]
#STATE#	: [8:15]
#STATE#	keyword: else [8:17..20]
in state #STATE#, possible rules are:
	chp_nondet_guarded_command_list: chp_guarded_command ':' . chp_guarded_command  (#RULE#)
acceptable tokens are: 
	'(' (shift)
	'#' (shift)
	'-' (shift)
	'~' (shift)
	ID (shift)
	FLOAT (shift)
	INT (shift)
	SCOPE (shift)
	BOOL_TRUE (shift)
	BOOL_FALSE (shift)
