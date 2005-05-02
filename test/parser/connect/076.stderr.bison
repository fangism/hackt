parse error: syntax error
parser stacks:
state	value
0	list<(root_item)>: (process-prototype) ... [3:1--8:9]
22	(type-ref) [10:1..9]
35	list<(declaration-id)>: (declaration-array): identifier: plist<(range)>: (range) ... ... [10:11..16]
112	( [10:18]
in state 112, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (102)
	instance_id_list: instance_id_list . ',' instance_id_item  (103)
acceptable tokens are: 
	',' (shift)
	';' (shift)
