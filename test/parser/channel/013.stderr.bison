parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defchan [3:1..7]
111	identifier: mychan [3:9..14]
178	<: [3:16]
254	(chan-type) [3:19..22]
39	( [3:23]
118	list<(type-ref)>: (type-ref) ... [3:24..30]
192	[ [3:31]
in state 192, possible rules are:
	data_type_ref_list_in_parens: '(' data_type_ref_list . ')'  (67)
	data_type_ref_list: data_type_ref_list . ',' data_type_ref  (68)
acceptable tokens are: 
	')' (shift)
	',' (shift)
