parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: deftype [3:1..7]
111	identifier: my_precious [3:9..19]
178	; [3:20]
in state 178, possible rules are:
	declare_datatype_proto: optional_template_specification DEFTYPE ID . DEFINEOP data_type_ref data_param_decl_list_in_parens ';'  (69)
	defdatatype: optional_template_specification DEFTYPE ID . DEFINEOP data_type_ref data_param_decl_list_in_parens '{' set_body get_body '}'  (70)
acceptable tokens are: 
	DEFINEOP (shift)
