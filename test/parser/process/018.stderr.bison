parse error: syntax error
parser stacks:
state	value
0	(template-formal-decl-list-pair) [3:10..41]
30	keyword: defproc [4:1..7]
113	identifier: barbar [4:9..14]
183	$end
in state 183, possible rules are:
	declare_proc_proto: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens ';'  (31)
	defproc: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (32)
acceptable tokens are: 
	'(' (shift)
