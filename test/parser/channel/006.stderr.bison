At: "-stdin-":3:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defchan [3:1..7]
#STATE#	identifier: mychan [3:9..14]
#STATE#	<: [3:16]
#STATE#	(chan-type) [3:19..28]
#STATE#	( [3:30]
#STATE#	paramtype: pint [3:31..34]
in state #STATE#, possible rules are:
	data_param_decl_list_in_parens: '(' . data_param_decl_list ')'  (#RULE#)
acceptable tokens are: 
	ID (shift)
	SCOPE (shift)
	INT_TYPE (shift)
	BOOL_TYPE (shift)
