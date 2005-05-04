parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
110	identifier: myinverter [3:9..18]
178	list<(port-formal-decl)>: (port-formal-decl) ... [3:19..34]
249	{ [3:36]
286	list<(def-body-item)>: (prs-body) ... [4:1--7:1]
357	list<(expr)>: (array-concatenation) ... [8:1..11]
51	} [9:1]
in state 51, possible rules are:
	instance_item: alias_list . '=' expr ';'  (97)
	alias_list: alias_list . '=' complex_aggregate_reference  (112)
acceptable tokens are: 
	'=' (shift)
