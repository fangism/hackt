Parse error: base reference of function call must be an id_expr, but got: (index-expr) at [6:6..10]
At: "bad-loop-02.hac":6:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	( [6:1]
#STATE#	; [6:2]
#STATE#	identifier: i [6:3]
#STATE#	: [6:4]
#STATE#	(range) [6:12..13]
#STATE#	; [6:14]
in state #STATE#, possible rules are:
	loop_instantiation: '(' ';' ID ':' range . ':' instance_management_list ')'  (#RULE#)
acceptable tokens are: 
	':' (shift)
