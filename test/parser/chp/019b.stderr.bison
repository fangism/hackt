At: "019b.hac":11:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [4:1..7]
#STATE#	identifier: one_shot [4:9..16]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [4:17..46]
#STATE#	{ [4:48]
#STATE#	list<(def-body-item)>: (instance-decl) ... [5:1..5]
#STATE#	keyword: chp [6:1..3]
#STATE#	{ [6:5]
#STATE#	list<(chp-statement)>: list<(chp-statement)>: (chp-send) ... ... [7:2--10:11]
#STATE#	; [10:12]
#STATE#	(id-expr): R [11:2]
#STATE#	! [11:3]
#STATE#	( [11:4]
#STATE#	list<(expr)>: (id-expr): z ... [11:5]
#STATE#	< [11:7]
in state #STATE#, possible rules are:
	expr_list_in_parens: '(' expr_list . ')'  (#RULE#)
	expr_list: expr_list . ',' expr  (#RULE#)
acceptable tokens are: 
	')' (shift)
	',' (shift)
