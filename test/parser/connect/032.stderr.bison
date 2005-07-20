parse error: syntax error
parser stacks:
state	value
0	list<(root_item)>: (process-prototype) ... [3:1--5:33]
22	(type-ref) [7:1..6]
36	list<(declaration-id)>: (declaration-id): identifier: load ... [7:8..11]
117	. [7:12]
in state 117, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (116)
	instance_id_list: instance_id_list . ',' instance_id_item  (117)
acceptable tokens are: 
	',' (shift)
	';' (shift)
