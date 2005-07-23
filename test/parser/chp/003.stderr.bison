parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [3:1..7]
115	identifier: empty [3:9..13]
186	list<(port-formal-decl)>: (port-formal-decl) ... [3:14..43]
262	{ [3:45]
307	keyword: chp [4:1..3]
233	{ [4:5]
279	} [5:1]
in state 279, possible rules are:
	language_body: CHP_LANG '{' . chp_body '}'  (134)
acceptable tokens are: 
	'[' (shift)
	ID (shift)
	SCOPE (shift)
	BEGINLOOP (shift)
	LOG (shift)
