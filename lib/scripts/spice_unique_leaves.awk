#!/usr/bin/awk -f
# "spice_unique_leaves.awk"
# prints names of leaf cells with transistors

# input: hierarchical spice netlist
# assumes that dependent subcircuits are defined before they are used
# [topologically sorted precondition]

BEGIN {
#	debug = 1;
	have_transistor = 0;
}

function begin_subckt() {
	target = $2;
#	cells[target] = 1;
}

# assumes no nested subcircuit definitions!
/^\.subckt/ {
	begin_subckt();
}
/^\.SUBCKT/ {
	begin_subckt();
}

function end_subckt() {
	if (have_transistor)
		print target;
	have_transistor = 0;
}

/^\.ends/ {
	end_subckt();
}
/^\.ENDS/ {
	end_subckt();
}

# also count transistors
/^[mM]/ {
if (length(target)) {
	have_transistor = 1;
}
# else is top-level instance
}

