parse error: syntax error
parser stacks:
state	value
	(null) 
0	keyword: namespace [3:1..9]
9	identifier: close_me [3:11..18]
94	{ [3:20]
161	(null) 
30	$end
in state 30, possible rules are:
	type_alias: optional_template_specification . TYPEDEF physical_type_ref ID ';'  (24)
	declare_proc_proto: optional_template_specification . def_or_proc ID optional_port_formal_decl_list_in_parens ';'  (30)
	defproc: optional_template_specification . def_or_proc ID optional_port_formal_decl_list_in_parens '{' optional_definition_body '}'  (31)
	declare_datatype_proto: optional_template_specification . DEFTYPE ID DEFINEOP data_type_ref data_param_decl_list_in_parens ';'  (66)
	defdatatype: optional_template_specification . DEFTYPE ID DEFINEOP data_type_ref data_param_decl_list_in_parens '{' set_body get_body '}'  (67)
	declare_chan_proto: optional_template_specification . DEFCHAN ID DEFINEOP base_chan_type data_param_decl_list_in_parens ';'  (74)
	defchan: optional_template_specification . DEFCHAN ID DEFINEOP base_chan_type data_param_decl_list_in_parens '{' send_body recv_body '}'  (75)
acceptable tokens are: 
	DEFINE (shift)
	DEFPROC (shift)
	DEFCHAN (shift)
	DEFTYPE (shift)
	TYPEDEF (shift)
