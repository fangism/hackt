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
298	(chp-det-sel) [8:4..30]
372	[] [8:32]
in state 372, possible rules are:
	chp_selection: '[' chp_matched_det_guarded_command_list . ']'  (146)
acceptable tokens are: 
	']' (shift)
