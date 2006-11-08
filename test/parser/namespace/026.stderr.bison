At: "026.hac":6:
parse error: syntax error
parser stacks:
state	value
#STATE#	(null) 
#STATE#	keyword: namespace [3:1..9]
#STATE#	identifier: close_me [3:11..18]
#STATE#	{ [3:20]
#STATE#	(null) 
#STATE#	$end
in state #STATE#, possible rules are:
	type_alias: optional_template_specification . TYPEDEF physical_type_ref ID ';'  (#RULE#)
	declare_proc_proto: optional_template_specification . def_or_proc ID optional_port_formal_decl_list_in_parens ';'  (#RULE#)
	defproc: optional_template_specification . def_or_proc ID optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (#RULE#)
	declare_datatype_proto: optional_template_specification . DEFTYPE ID DEFINEOP data_type_ref data_param_decl_list_in_parens ';'  (#RULE#)
	defdatatype: optional_template_specification . DEFTYPE ID DEFINEOP data_type_ref data_param_decl_list_in_parens '{' optional_definition_body set_body get_body '}'  (#RULE#)
	declare_chan_proto: optional_template_specification . DEFCHAN ID DEFINEOP base_chan_type data_param_decl_list_in_parens ';'  (#RULE#)
	defchan: optional_template_specification . DEFCHAN ID DEFINEOP base_chan_type data_param_decl_list_in_parens '{' optional_definition_body send_body recv_body '}'  (#RULE#)
acceptable tokens are: 
	DEFINE (shift)
	DEFPROC (shift)
	DEFCHAN (shift)
	DEFTYPE (shift)
	TYPEDEF (shift)
