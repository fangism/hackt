At: "019b.hac":11:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(process-prototype) [4:1..46]
#STATE#	{ [4:48]
#STATE#	list<(def-body-item)>: (instance-decl) ... [5:1..5]
#STATE#	keyword: chp [6:1..3]
#STATE#	{ [6:5]
#STATE#	list<(chp-statement)>: list<(chp-statement)>: (chp-send) ... ... [7:2--10:11]
#STATE#	; [10:12]
#STATE#	(id-expr): R [11:2]
#STATE#	! [11:3]
#STATE#	( [11:4]
#STATE#	list<(expr)>: (array-concatenation) ... [11:5]
#STATE#	< [11:7]
in state #STATE#, possible rules are:
	connection_actuals_list: '(' complex_aggregate_reference_list . ')'  (#RULE#)
	complex_aggregate_reference_list: complex_aggregate_reference_list . ',' optional_complex_aggregate_reference  (#RULE#)
acceptable tokens are: 
	')' (shift)
	',' (shift)
