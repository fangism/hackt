At: "035.hac":10:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	list<(root_item)>: (process-definition) ... [7:1..21]
#STATE#	(null) 
#STATE#	keyword: deftype [9:1..7]
#STATE#	identifier: my_data [9:9..15]
#STATE#	<: [9:17]
#STATE#	(type-ref) [9:20..25]
#STATE#	list<(port-formal-decl)>: (port-formal-decl) ... [9:27..44]
#STATE#	{ [9:46]
#STATE#	list<(def-body-item)>: ... [0:0]
#STATE#	datatype: bool [10:2..5]
in state #STATE#, possible rules are:
	defdatatype: optional_template_specification DEFTYPE ID DEFINEOP data_type_ref optional_port_formal_decl_list_in_parens '{' optional_datatype_body . set_body get_body '}'  (#RULE#)
acceptable tokens are: 
	SET (shift)
