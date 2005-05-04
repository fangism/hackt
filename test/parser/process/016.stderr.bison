parse error: syntax error
parser stacks:
state	value
0	keyword: template [3:1..8]
12	< [3:10]
99	list<(template-formal-decl)>: (template-formal-decl) ... [3:11..24]
166	keyword: defproc [4:1..7]
in state 166, possible rules are:
	template_formal_decl_list_in_angles: '<' template_formal_decl_list . '>'  (35)
	template_formal_decl_list: template_formal_decl_list . ';' template_formal_decl  (38)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
