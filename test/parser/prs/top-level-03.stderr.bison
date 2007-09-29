At: "top-level-03.hac":12:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (namespace-body: foo) ... [7:1--9:1]
#STATE#	keyword: prs [11:1..3]
#STATE#	(null) 
#STATE#	{ [11:5]
#STATE#	(prs-literal) [12:2..4]
#STATE#	:: [12:5]
in state #STATE#, possible rules are:
	single_prs: prs_expr . prs_arrow prs_literal_base dir  (#RULE#)
acceptable tokens are: 
	IMPLIES (shift)
	RARROW (shift)
