At: "-stdin-":9:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(template-formal-decl-list-pair) [5:10..17]
#STATE#	keyword: defproc [6:1..7]
#STATE#	identifier: null [6:9..12]
#STATE#	list<(port-formal-decl)>: ... [6:13..14]
#STATE#	{ [6:16]
#STATE#	list<(def-body-item)>: (instance-decl) ... [7:1..9]
#STATE#	( [8:1]
#STATE#	; [8:2]
#STATE#	identifier: i [8:3]
#STATE#	: [8:4]
#STATE#	(range) [8:5]
#STATE#	: [8:6]
#STATE#	keyword: prs [9:1..3]
in state #STATE#, possible rules are:
	loop_instantiation: '(' ';' ID ':' range ':' . instance_management_list ')'  (#RULE#)
acceptable tokens are: 
	'{' (shift)
	'[' (shift)
	'(' (shift)
	'-' (shift)
	'~' (shift)
	'!' (shift)
	ID (shift)
	FLOAT (shift)
	INT (shift)
	STRING (shift)
	SCOPE (shift)
	CHANNEL (shift)
	BOOL_TRUE (shift)
	BOOL_FALSE (shift)
	INT_TYPE (shift)
	BOOL_TYPE (shift)
	PINT_TYPE (shift)
	PBOOL_TYPE (shift)
	PREAL_TYPE (shift)