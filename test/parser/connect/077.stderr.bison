parse error: syntax error
parser stacks:
state	value
0	list<(root_item)>: (process-prototype) ... [3:1--8:7]
22	(type-ref) [10:1..9]
36	list<(declaration-id)>: (declaration-array): identifier: plist<(range)>: (range) ... ... [10:11..13]
117	( [10:15]
in state 117, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (116)
	instance_id_list: instance_id_list . ',' instance_id_item  (117)
acceptable tokens are: 
	',' (shift)
	';' (shift)
