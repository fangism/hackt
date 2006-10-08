At: "-stdin-":7:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-prototype) ... [5:1..13]
#STATE#	(qualified-id) [7:1..3]
#STATE#	list<(expr)>: (array-concatenation) ... [7:4..7]
#STATE#	< [7:8]
#STATE#	> [7:9]
in state #STATE#, possible rules are:
	expr_list_in_angles: '<' . expr_list '>'  (#RULE#)
acceptable tokens are: 
	'(' (shift)
	'-' (shift)
	'~' (shift)
	'!' (shift)
	ID (shift)
	FLOAT (shift)
	INT (shift)
	STRING (shift)
	SCOPE (shift)
	BOOL_TRUE (shift)
	BOOL_FALSE (shift)
