parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
114	identifier: empty [3:9..13]
183	list<(port-formal-decl)>: (port-formal-decl) ... [3:14..43]
258	{ [3:45]
296	keyword: chp [4:1..3]
234	{ [4:5]
273	(id-expr): R [5:2]
319	! [5:3]
385	bool: false [5:4..8]
in state 385, possible rules are:
	chp_send: member_index_expr '!' . expr_list_in_parens  (160)
acceptable tokens are: 
	'(' (shift)
