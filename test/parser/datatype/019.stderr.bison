parse error: syntax error
parser stacks:
state	value
	(null) 
0	keyword: enum [3:1..4]
13	identifier: foo [3:6..8]
101	, [3:9]
in state 101, possible rules are:
	declare_enum: ENUM ID . ';'  (70)
	defenum: ENUM ID . '{' enum_member_list '}'  (71)
acceptable tokens are: 
	'{' (shift)
	';' (shift)
