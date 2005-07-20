parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
115	identifier: myinverter [3:9..18]
186	list<(port-formal-decl)>: (port-formal-decl) ... [3:19..34]
262	{ [3:36]
307	list<(def-body-item)>: (prs-body) ... [4:1--7:1]
383	list<(expr)>: (array-concatenation) ... [8:1..11]
54	} [9:1]
in state 54, possible rules are:
	instance_item: alias_list . '=' expr ';'  (111)
	alias_list: alias_list . '=' complex_aggregate_reference  (128)
acceptable tokens are: 
	'=' (shift)
