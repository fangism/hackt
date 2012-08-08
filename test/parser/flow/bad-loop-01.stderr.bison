Parse error: base reference of function call must be an id_expr, but got: (index-expr) at [8:6..10]
At: "bad-loop-01.hac":8:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (instance-decl) ... [6:1--7:8]
#STATE#	( [8:1]
#STATE#	; [8:2]
#STATE#	identifier: i [8:3]
#STATE#	: [8:4]
#STATE#	(range) [8:12..20]
#STATE#	; [8:21]
in state #STATE#, possible rules are:
	loop_instantiation: '(' ';' ID ':' range . ':' instance_management_list ')'  (#RULE#)
acceptable tokens are: 
	':' (shift)
