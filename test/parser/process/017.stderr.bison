parse error: syntax error
parser stacks:
state	value
	(null) 
0	keyword: template [3:1..8]
12	< [3:10]
99	(node_list): (template-formal-decl) ... [3:11..45]
164	keyword: defproc [4:1..7]
in state 164, possible rules are:
	template_formal_decl_list_in_angles: '<' template_formal_decl_list . '>'  (34)
	template_formal_decl_list: template_formal_decl_list . ';' template_formal_decl  (35)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
