parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
109	identifier: myinverter [3:9..18]
174	(node_list): (port-formal-decl) ... [3:19..34]
245	{ [3:36]
282	(node_list): (prs-body) ... [4:1--7:1]
353	(node_list): (array-concatenation) ... [8:1..11]
51	} [9:1]
in state 51, possible rules are:
	instance_item: alias_list . '=' expr ';'  (94)
	alias_list: alias_list . '=' complex_aggregate_reference  (109)
acceptable tokens are: 
	'=' (shift)
