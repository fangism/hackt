parse error: syntax error
parser stacks:
state	value
0	keyword: template [3:1..8]
12	< [3:10]
99	paramtype: pint [3:11..14]
168	list<(template-formal-id)>: (template-formal-id): identifier: milk ... [3:16..19]
241	, [3:20]
276	keyword: defproc [4:1..7]
in state 276, possible rules are:
	template_formal_id_list: template_formal_id_list ',' . template_formal_id  (41)
acceptable tokens are: 
	ID (shift)
