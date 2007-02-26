At: "055.hac":8:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [3:1..7]
#STATE#	identifier: split_bool_soup [3:9..23]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [3:24..59]
#STATE#	{ [3:61]
#STATE#	list<(def-body-item)>: (instance-decl) ... [4:1..9]
#STATE#	keyword: chp [5:1..3]
#STATE#	{ [5:5]
#STATE#	*[ [6:2]
#STATE#	list<(chp-statement)>: list<(chp-statement)>: (chp-receive) ... ... [7:3..7]
#STATE#	; [7:8]
#STATE#	[ [8:3]
#STATE#	keyword: else [8:5..8]
in state #STATE#, possible rules are:
	chp_wait: '[' . chp_guard_expr ']'  (#RULE#)
	chp_selection: '[' . chp_matched_det_guarded_command_list ']'  (#RULE#)
	             | '[' . chp_nondet_guarded_command_list ']'  (#RULE#)
	chp_metaloop_selection: '[' . ':' ID ':' range ':' chp_guarded_command ']'  (#RULE#)
	                      | '[' . THICKBAR ID ':' range ':' chp_guarded_command ']'  (#RULE#)
acceptable tokens are: 
	'(' (shift)
	':' (shift)
	'#' (shift)
	'-' (shift)
	'~' (shift)
	ID (shift)
	FLOAT (shift)
	INT (shift)
	SCOPE (shift)
	THICKBAR (shift)
	BOOL_TRUE (shift)
	BOOL_FALSE (shift)
