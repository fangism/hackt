parse error: syntax error
parser stacks:
state	value
0	list<(root_item)>: (process-prototype) ... [3:1--5:34]
22	(type-ref) [7:1..6]
35	list<(declaration-id)>: (declaration-id): identifier: load ... [7:8..11]
112	. [7:12]
in state 112, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (102)
	instance_id_list: instance_id_list . ',' instance_id_item  (103)
acceptable tokens are: 
	',' (shift)
	';' (shift)
