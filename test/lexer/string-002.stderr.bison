At: "-stdin-":7:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(expr)>: (array-concatenation) ... [5:63]
#STATE#	$end
in state #STATE#, possible rules are:
	instance_item: alias_list . '=' expr ';'  (#RULE#)
	alias_list: alias_list . '=' complex_aggregate_reference  (#RULE#)
acceptable tokens are: 
	'=' (shift)
