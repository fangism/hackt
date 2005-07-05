parse error: syntax error
parser stacks:
state	value
0	keyword: template [4:1..8]
12	list<(template-formal-decl)>: (template-formal-decl) ... [4:10..17]
106	< [4:18]
177	list<(template-formal-decl)>: (template-formal-decl) ... [4:19..24]
252	= [4:25]
in state 252, possible rules are:
	template_formal_decl_nodefault_list_in_angles: '<' template_formal_decl_nodefault_list . '>'  (36)
	template_formal_decl_nodefault_list: template_formal_decl_nodefault_list . ';' template_formal_decl_nodefault  (41)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
