At: "size-02.hac":6:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [4:1..7]
#STATE#	identifier: size_test [4:9..17]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [4:19..29]
#STATE#	{ [4:31]
#STATE#	keyword: prs [5:1..3]
#STATE#	{ [5:5]
#STATE#	(prs-literal) [6:2]
#STATE#	=> [6:4]
#STATE#	(prs-literal) [6:7]
#STATE#	< [6:8]
in state #STATE#, possible rules are:
	single_prs: prs_expr prs_arrow prs_literal_base . dir  (#RULE#)
acceptable tokens are: 
	'+' (shift)
	'-' (shift)
