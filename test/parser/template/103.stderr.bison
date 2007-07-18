At: "103.hac":9:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-definition) ... [6:1..17]
#STATE#	(template-formal-decl-list-pair) [8:10..17]
#STATE#	keyword: defproc [9:1..7]
#STATE#	identifier: bar [9:9..11]
#STATE#	( [9:12]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [9:13..20]
#STATE#	< [9:21]
in state #STATE#, possible rules are:
	optional_port_formal_decl_list_in_parens: '(' port_formal_decl_list . ')'  (#RULE#)
	port_formal_decl_list: port_formal_decl_list . ';' port_formal_decl  (#RULE#)
acceptable tokens are: 
	')' (shift)
	';' (shift)
