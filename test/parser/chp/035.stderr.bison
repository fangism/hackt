parse error: syntax error
parser stacks:
state	value
0	(null) 
30	keyword: defproc [4:1..7]
114	identifier: sum_diff [4:9..16]
183	( [4:17]
257	(chan-type) [4:18..27]
295	list<(port-formal-id)>: (port-formal-id): identifier: L1 ... [4:29..30]
364	, [4:31]
427	keyword: chan [4:33..36]
in state 427, possible rules are:
	port_formal_id_list: port_formal_id_list ',' . port_formal_id  (48)
acceptable tokens are: 
	ID (shift)
