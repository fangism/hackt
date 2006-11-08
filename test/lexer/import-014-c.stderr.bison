At: "import-014-c.hac":17:
parse error: syntax error
parser stacks:
state	value
#STATE#	(imported-root-list) [7:1--8:27]
#STATE#	(null) 
#STATE#	keyword: defproc [10:1..7]
#STATE#	identifier: problem [10:9..15]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [10:17..48]
#STATE#	{ [11:1]
#STATE#	list<(def-body-item)>: (instance-decl) ... [12:2--13:15]
#STATE#	keyword: prs [16:2..4]
#STATE#	{ [16:6]
#STATE#	(prs-literal) [17:3..5]
#STATE#	+ [17:6]
in state #STATE#, possible rules are:
	single_prs: prs_expr . prs_arrow relative_member_index_expr dir  (#RULE#)
acceptable tokens are: 
	IMPLIES (shift)
	RARROW (shift)
