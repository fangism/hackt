At: "077.hac":10:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-prototype) ... [3:1--8:7]
#STATE#	(type-ref) [10:1..9]
#STATE#	list<(declaration-id)>: (declaration-array): identifier: plist<(range)>: (range) ... ... [10:11..13]
#STATE#	( [10:15]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
