At: "056.hac":8:
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
#STATE#	(chp-det-sel) [8:4..30]
#STATE#	[] [8:32]
in state #STATE#, possible rules are:
	chp_selection: '[' chp_matched_det_guarded_command_list . ']'  (#RULE#)
acceptable tokens are: 
	']' (shift)
