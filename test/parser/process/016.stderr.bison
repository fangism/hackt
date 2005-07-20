parse error: syntax error
parser stacks:
state	value
0	keyword: template [3:1..8]
12	< [3:10]
104	list<(template-formal-decl)>: (template-formal-decl) ... [3:11..24]
174	keyword: defproc [4:1..7]
in state 174, possible rules are:
	template_formal_decl_list_in_angles: '<' template_formal_decl_list . '>'  (35)
	template_formal_decl_list: template_formal_decl_list . ';' template_formal_decl  (39)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
