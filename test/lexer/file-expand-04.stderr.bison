At: "file-expand-04.hac":5:
parse error: syntax error
parser stacks:
state	value
#STATE#	(imported-root-list) [3:1..21]
#STATE#	(type-ref) [5:1..4]
#STATE#	list<(declaration-id)>: (declaration-id): identifier: error ... [5:6..10]
#STATE#	identifier: here [5:12..15]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
