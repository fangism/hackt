parse error: syntax error
parser stacks:
state	value
0	(node_list): (process-prototype) ... [3:1--5:34]
22	(type-ref) [7:1..6]
35	(node_list): (declaration-id): identifier: load ... [7:8..11]
111	. [7:12]
in state 111, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (99)
	instance_id_list: instance_id_list . ',' instance_id_item  (100)
acceptable tokens are: 
	',' (shift)
	';' (shift)
