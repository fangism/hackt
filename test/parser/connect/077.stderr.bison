parse error: syntax error
parser stacks:
state	value
	(null) 
0	(node_list): (process-prototype) ... [3:1--8:9]
22	(type-ref) [10:1..9]
35	(node_list): (declaration-array): identifier: p(node_list): (range) ... ... [10:11..13]
111	( [10:15]
in state 111, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (99)
	instance_id_list: instance_id_list . ',' instance_id_item  (100)
acceptable tokens are: 
	',' (shift)
	';' (shift)
