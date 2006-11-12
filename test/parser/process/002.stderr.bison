At: "002.hac":3:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [3:1..7]
#STATE#	identifier: fang_again [3:9..18]
#STATE#	< [3:19]
in state #STATE#, possible rules are:
	declare_proc_proto: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens ';'  (#RULE#)
	defproc: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (#RULE#)
acceptable tokens are: 
	'(' (shift)
