At: "034.hac":13:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-definition) ... [7:1--10:1]
#STATE#	(null) 
#STATE#	keyword: deftype [12:1..7]
#STATE#	identifier: my_data [12:9..15]
#STATE#	<: [12:17]
#STATE#	(type-ref) [12:20..25]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [12:27..44]
#STATE#	{ [12:46]
#STATE#	list<(def-body-item)>: ... [0:0]
#STATE#	datatype: bool [13:2..5]
in state #STATE#, possible rules are:
	defdatatype: optional_template_specification DEFTYPE ID DEFINEOP data_type_ref optional_port_formal_decl_list_in_parens '{' optional_datatype_body . set_body get_body '}'  (#RULE#)
acceptable tokens are: 
	SET (shift)
