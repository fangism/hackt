#!/usr/bin/awk -f
# "spice_cell_count.awk"
# cumulative cell count, assuming 1 instance of the very last subcircuit
# if no top-level instances are found

# input: hierarchical spice netlist
# assumes that dependent subcircuits are defined before they are used
# [topologically sorted precondition]

# output 3 fields (tab-delimited):
# supercelltype subcelltype count

BEGIN {
#	debug = 1;
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

function debug_print(str) {
if (debug) {
	print str;	# or comment this out
}
}

# return a string listing all subsubcell types (unique)
function count_subcells(tgt, cell, cnt, 
	# local vars
	i, t, ntoks, tl, v, c) {
	subs[cell];
	cell_count[tgt " " cell] += cnt;
	debug_print("#\t\t" tgt " " cell " : " cnt);
	debug_print("# lookup subcell types of " cell ": " sub_list[cell]);
	ntoks = split(sub_list[cell], tl);
	for (i=1; i<=ntoks; ++i) {
		t = tl[i];
		subs[t];
		c = cell_count[cell " " t];
		v = c *cnt;
		cell_count[tgt " " t] += v;
		debug_print("#\t\t" tgt " " cell " += (" c "*" cnt ") " v);
	}
	# list all subsubcell types ever seen (unique)
}

function end_subckt() {
	debug_print("# processed subcircuit: " target);
	# inherit type's subprocess count
	for (s in subs) { delete subs[s]; }
	for (l in local) {
		count = local[l];	# multiplier
		debug_print("#\thave " count " of " l);
		count_subcells(target, l, count);
	}
	# list of recursive subcell types (unique)
	ret = "";
	for (s in subs) {
		ret = ret " " s;
	}
	debug_print("# subcell types of " target ": " ret);
	sub_list[target] = ret;

	# debug: summarize
	summarize_cell(target);

	last_target = target;
	target = "";
	for (l in local) { delete local[l]; }
}

/^\.ends/ {
	end_subckt();
}
/^\.ENDS/ {
	end_subckt();
}

/^[xX]/ {
if (length(target)) {
	ntoks = split($0, toks);
	type = toks[ntoks];
	++local[type];
}
# else is top-level instance
}

# also count transistors, sort by size and type
/^[mM]/ {
if (length(target)) {
	fet = $6;
	# scan parameters for width
	ntoks = split($0, toks);
	for (i=7; i<=ntoks; ++i) {
		if (match(toks[i], "[Ww]=")) {
			ntoks = split(toks[i], toks, "=");
			W = toks[2];
			break;
		}
	}
	subtype = "<TRANSISTOR:" fet ":" W ">";
	type = "<TRANSISTOR>";	# includes all types/sizes
	++local[subtype];
	++local[type];
}
# else is top-level instance
}

function summarize_cell(tgt, 
	# local vars
	s, i, ntoks, sl) {
	ntoks = split(sub_list[tgt], sl);
	for (i=1; i<=ntoks; ++i) {
		s = sl[i];
		print tgt "\t" s "\t" cell_count[tgt " " s];
	}
}

# END { summarize_cell(last_target); }
