At: "act-007.hac":9:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-prototype) ... [5:1--7:9]
#STATE#	(type-ref) [9:1..3]
#STATE#	list<(declaration-id)>: (declaration-array): identifier: flist<(range)>: (range) ... ... [9:5..7]
#STATE#	( [9:9]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
