At: "018.hac":12:
parse error: syntax error
parser stacks:
state	value
#STATE#	(imported-root-list) [8:1..18]
#STATE#	list<(root_item)>: (instance-decl) ... [10:1..6]
#STATE#	(null) 
#STATE#	(null) 
#STATE#	(imported-root) [12:1..18]
in state #STATE#, possible rules are:
	type_alias: optional_export optional_template_specification . TYPEDEF physical_type_ref ID ';'  (#RULE#)
	declare_proc_proto: optional_export optional_template_specification . def_or_proc ID optional_port_formal_decl_list_in_parens  (#RULE#)
	declare_datatype_proto: optional_export optional_template_specification . DEFTYPE ID DEFINEOP data_type_ref optional_port_formal_decl_list_in_parens  (#RULE#)
	declare_chan_proto: optional_export optional_template_specification . DEFCHAN ID DEFINEOP base_chan_type optional_port_formal_decl_list_in_parens  (#RULE#)
acceptable tokens are: 
	DEFINE (shift)
	DEFPROC (shift)
	DEFCHAN (shift)
	DEFTYPE (shift)
	TYPEDEF (shift)
