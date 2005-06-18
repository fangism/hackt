parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
114	identifier: split_bool_soup [3:9..23]
185	list<(port-formal-decl)>: (port-formal-decl) ... [3:24..59]
253	{ [3:61]
291	list<(def-body-item)>: (instance-decl) ... [4:1..9]
361	keyword: chp [5:1..3]
229	{ [5:5]
268	*[ [6:2]
299	list<(chp-statement)>: (chp-comm-list) ... [7:3..7]
303	; [7:8]
396	[ [8:3]
298	keyword: else [8:5..8]
in state 298, possible rules are:
	chp_wait: '[' . chp_guard_expr ']'  (145)
	chp_selection: '[' . chp_matched_det_guarded_command_list ']'  (146)
	             | '[' . chp_nondet_guarded_command_list ']'  (147)
acceptable tokens are: 
	'(' (shift)
	'-' (shift)
	'~' (shift)
	ID (shift)
	FLOAT (shift)
	INT (shift)
	SCOPE (shift)
	BOOL_TRUE (shift)
	BOOL_FALSE (shift)
