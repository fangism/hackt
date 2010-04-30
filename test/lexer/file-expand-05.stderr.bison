At: "file-expand-05.hac":3:
At: "wrapper.hac":4:
parse error: syntax error
parser stacks:
state	value
#STATE#	(imported-root) [3:1..21]
#STATE#	%{ [0:2]
#STATE#	(imported-root-list) [1:1--2:20]
#STATE#	(type-ref) [4:1..4]
#STATE#	list<(declaration-id)>: (declaration-id): identifier: error ... [4:6..10]
#STATE#	identifier: here [4:12..15]
in state #STATE#, possible rules are:
	type_instance_declaration: type_id instance_id_list . ';'  (#RULE#)
	instance_id_list: instance_id_list . ',' instance_id_item  (#RULE#)
acceptable tokens are: 
	',' (shift)
	';' (shift)
