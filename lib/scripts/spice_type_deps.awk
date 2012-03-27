#!/usr/bin/awk -f
# "spice_type_deps.awk"
#	by Fang
# scans a spice netlist and emits type dependencies
# recommend piping the output through sort -u (unique)

# second argument of subckt line is the defining type
/^.subckt/ { target = $2; }

# last position is the instance type
/^[xX]/ {
	ntoks = split($0, toks);
	print target ": " toks[ntoks];
}

