parse error: syntax error
parser stacks:
state	value
0	list<(root_item)>: (process-prototype) ... [5:1..13]
22	(qualified-id) [7:1..3]
58	list<(expr)>: int: 11 ... [7:4..7]
123	< [7:8]
187	> [7:9]
in state 187, possible rules are:
	shift_expr_list_in_angles: '<' . shift_expr_list '>'  (322)
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
