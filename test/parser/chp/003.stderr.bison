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
273	} [5:1]
in state 273, possible rules are:
	language_body: CHP_LANG '{' . chp_body '}'  (125)
acceptable tokens are: 
	'[' (shift)
	ID (shift)
	SCOPE (shift)
	BEGINLOOP (shift)
	SKIP (shift)
	LOG (shift)
