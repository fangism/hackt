parse error: syntax error
parser stacks:
state	value
0	keyword: template [3:1..8]
12	list<(template-formal-decl)>: (template-formal-decl) ... [3:10..18]
101	< [3:20]
169	> [3:21]
in state 169, possible rules are:
	template_formal_decl_list_in_angles: '<' . template_formal_decl_list '>'  (35)
acceptable tokens are: 
	PINT_TYPE (shift)
	PBOOL_TYPE (shift)
