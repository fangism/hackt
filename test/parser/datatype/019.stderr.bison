At: "-stdin-":3:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	keyword: enum [3:1..4]
#STATE#	identifier: foo [3:6..8]
#STATE#	, [3:9]
in state #STATE#, possible rules are:
	declare_enum: ENUM ID . ';'  (#RULE#)
	defenum: ENUM ID . '{' enum_member_list '}'  (#RULE#)
acceptable tokens are: 
	'{' (shift)
	';' (shift)
