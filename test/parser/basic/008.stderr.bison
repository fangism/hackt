At: "-stdin-":9:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	(null) 
#STATE#	keyword: defproc [3:1..7]
#STATE#	identifier: myinverter [3:9..18]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [3:19..34]
#STATE#	{ [3:36]
#STATE#	list<(def-body-item)>: (prs-body) ... [4:1--7:1]
#STATE#	(array-concatenation) [8:1..11]
#STATE#	} [9:1]
in state #STATE#, possible rules are:
	nonempty_alias_list: complex_aggregate_reference . '=' complex_aggregate_reference  (#RULE#)
acceptable tokens are: 
	'=' (shift)
