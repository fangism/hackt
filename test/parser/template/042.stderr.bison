At: "042.hac":4:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	keyword: template [4:1..8]
#STATE#	list<(template-formal-decl)>: (template-formal-decl) ... [4:10..17]
#STATE#	< [4:18]
#STATE#	list<(template-formal-decl)>: (template-formal-decl) ... [4:19..24]
#STATE#	= [4:25]
in state #STATE#, possible rules are:
	template_formal_decl_nodefault_list_in_angles: '<' template_formal_decl_nodefault_list . '>'  (#RULE#)
	template_formal_decl_nodefault_list: template_formal_decl_nodefault_list . ';' template_formal_decl_nodefault  (#RULE#)
acceptable tokens are: 
	'>' (shift)
	';' (shift)
