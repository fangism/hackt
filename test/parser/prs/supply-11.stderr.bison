At: "supply-11.hac":5:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(process-prototype) [4:1..22]
#STATE#	{ [4:24]
#STATE#	keyword: prs [5:1..3]
#STATE#	< [5:5]
#STATE#	list<(inst-ref-expr)>: ... [0:0]
#STATE#	! [5:6]
in state #STATE#, possible rules are:
	member_index_expr_list_in_angles: '<' member_index_expr_pair . '>'  (#RULE#)
	                                | '<' member_index_expr_pair . '|' member_index_expr_pair '>'  (#RULE#)
acceptable tokens are: 
	'>' (shift)
	'|' (shift)
