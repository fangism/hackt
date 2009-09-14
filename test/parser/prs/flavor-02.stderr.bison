At: "flavor-02.hac":12:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (instance-decl) ... [7:1--8:11]
#STATE#	(process-prototype) [10:1..31]
#STATE#	{ [10:33]
#STATE#	keyword: prs [11:1..3]
#STATE#	(null) 
#STATE#	{ [11:5]
#STATE#	(prs-literal) [12:2..10]
#STATE#	& [12:12]
#STATE#	(null) 
#STATE#	(prs-literal) [12:14]
#STATE#	< [12:15]
#STATE#	list<(expr)>: int: 10 ... [12:16..21]
#STATE#	: [12:22]
in state #STATE#, possible rules are:
	prs_literal_params_in_angles_optional: '<' prs_literal_params_optional . ';' generic_attribute_list '>'  (#RULE#)
acceptable tokens are: 
	';' (shift)
