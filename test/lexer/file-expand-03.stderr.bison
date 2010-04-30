At: "file-expand-03.hac":7:
parse error: syntax error
parser stacks:
state	value
#STATE#	(imported-root-list) [4:1--5:20]
#STATE#	(type-ref) [7:1..4]
#STATE#	list<(declaration-id)>: (declaration-id): identifier: error ... [7:6..10]
#STATE#	identifier: here [7:12..15]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
