parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-prototype) ... [3:1--5:33]
#STATE#	(type-ref) [7:1..6]
#STATE#	list<(declaration-id)>: (declaration-id): identifier: load ... [7:8..11]
#STATE#	. [7:12]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
