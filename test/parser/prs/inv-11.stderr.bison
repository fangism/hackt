parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
113	identifier: inv [3:9..11]
182	list<(port-formal-decl)>: (port-formal-decl) ... [3:12..22]
255	{ [3:24]
293	keyword: prs [4:1..3]
233	{ [4:5]
272	~ [5:2]
334	~ [5:3]
in state 334, possible rules are:
	prs_not: '~' . prs_unary_expr  (197)
acceptable tokens are: 
	'(' (shift)
	ID (shift)
	SCOPE (shift)
