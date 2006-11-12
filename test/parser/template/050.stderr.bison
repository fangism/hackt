At: "050.hac":3:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	keyword: template [3:1..8]
#STATE#	list<(template-formal-decl)>: ... [3:10..11]
#STATE#	< [3:12]
#STATE#	list<(template-formal-decl)>: (template-formal-decl) ... [3:13..21]
#STATE#	= [3:22]
in state #STATE#, possible rules are:
	template_formal_decl_nodefault_list_in_angles: '<' template_formal_decl_nodefault_list . '>'  (#RULE#)
	template_formal_decl_nodefault_list: template_formal_decl_nodefault_list . ';' template_formal_decl_nodefault  (#RULE#)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
