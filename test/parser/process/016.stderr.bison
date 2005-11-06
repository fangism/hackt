parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	keyword: template [3:1..8]
#STATE#	< [3:10]
#STATE#	list<(template-formal-decl)>: (template-formal-decl) ... [3:11..24]
#STATE#	keyword: defproc [4:1..7]
in state #STATE#, possible rules are:
	template_formal_decl_list_in_angles: '<' template_formal_decl_list . '>'  (#RULE#)
	template_formal_decl_list: template_formal_decl_list . ';' template_formal_decl  (#RULE#)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
