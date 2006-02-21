At: "-stdin-":7:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (instance-decl) ... [4:1--6:8]
#STATE#	(array-concatenation) [7:1]
#STATE#	= [7:3]
#STATE#	{ [7:5]
#STATE#	list<(expr)>: (array-concatenation) ... [7:6]
#STATE#	, [7:7]
#STATE#	} [7:9]
in state #STATE#, possible rules are:
	mandatory_complex_aggregate_reference_list: mandatory_complex_aggregate_reference_list ',' . complex_aggregate_reference  (#RULE#)
acceptable tokens are: 
	'{' (shift)
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
