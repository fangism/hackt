At: "062.hac":8:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(process-prototype) [3:1..47]
#STATE#	{ [3:49]
#STATE#	list<(def-body-item)>: (instance-decl) ... [4:1..12]
#STATE#	keyword: chp [5:1..3]
#STATE#	{ [5:5]
#STATE#	*[ [6:2]
#STATE#	(chp-det-sel) [6:5--7:19]
#STATE#	[] [8:2]
#STATE#	keyword: else [8:5..8]
in state #STATE#, possible rules are:
	chp_unmatched_det_guarded_command_list: chp_unmatched_det_guarded_command_list THICKBAR . chp_guarded_command  (#RULE#)
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
