parse error: syntax error
parser stacks:
state	value
0	keyword: template [3:1..8]
12	list<(template-formal-decl)>: ... [3:10..11]
106	< [3:12]
177	list<(template-formal-decl)>: (template-formal-decl) ... [3:13..21]
252	= [3:22]
in state 252, possible rules are:
	template_formal_decl_nodefault_list_in_angles: '<' template_formal_decl_nodefault_list . '>'  (36)
	template_formal_decl_nodefault_list: template_formal_decl_nodefault_list . ';' template_formal_decl_nodefault  (41)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
