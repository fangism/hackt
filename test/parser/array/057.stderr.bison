At: "057.hac":9:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (instance-decl) ... [6:1--7:6]
#STATE#	(type-ref) [9:1..4]
#STATE#	list<(declaration-id)>: (declaration-array): identifier: zlist<(range)>: (range) ... ... [9:6..8]
#STATE#	= [9:11]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
