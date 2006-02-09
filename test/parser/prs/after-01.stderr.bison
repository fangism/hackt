At: "-stdin-":7:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [4:1..7]
#STATE#	identifier: aftershave [4:9..18]
#STATE#	list<(port-formal-decl)>: ... [4:19..20]
#STATE#	{ [4:22]
#STATE#	list<(def-body-item)>: (instance-decl) ... [5:1..9]
#STATE#	keyword: prs [6:1..3]
#STATE#	{ [6:5]
#STATE#	(prs-literal) [7:1..5]
#STATE#	int: 12 [7:7]
in state #STATE#, possible rules are:
	single_prs: prs_expr . prs_arrow relative_member_index_expr dir  (#RULE#)
acceptable tokens are: 
	IMPLIES (shift)
	RARROW (shift)
