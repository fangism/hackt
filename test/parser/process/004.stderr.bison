parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
109	identifier: foo [3:9..11]
174	< [3:13]
in state 174, possible rules are:
	declare_proc_proto: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens ';'  (30)
	defproc: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (31)
acceptable tokens are: 
	'(' (shift)
