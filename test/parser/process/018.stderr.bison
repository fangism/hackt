parse error: syntax error
parser stacks:
state	value
	(null) 
0	(node_list): (template-formal-decl) ... [3:10..41]
30	keyword: defproc [4:1..7]
109	identifier: barbar [4:9..14]
174	$end
in state 174, possible rules are:
	declare_proc_proto: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens ';'  (30)
	defproc: optional_template_specification def_or_proc ID . optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (31)
acceptable tokens are: 
	'(' (shift)
