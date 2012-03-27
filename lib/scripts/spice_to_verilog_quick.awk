#!/usr/bin/awk -f
# "spice_to_verilog_quick.awk"
# crudely converts spice netlist to verilog modules
# by Fang

/^\.subckt/ || /^\.SUBCKT/ {
	# reset hashes and arrays
	for (p in ports) delete ports[p];
	for (w in wires) delete wires[w];
	for (x in insts) delete insts[x];
	xc = 0;		# instance count

	print "module " $2 " (";
	# not smart enough to infer directionality
	for ( i=3; i<=NF; ++i) {
		printf ("\tinout %s", $(i));
		ports[$(i)] = 1;
		if (i == NF) {
			print "\n);";
		} else {
			print ",";
		}
	}
}

/^\.ends/ || /^\.ENDS/ {
	for (i=0; i<xc; ++i) {
		print insts[i];
	}
	print "endmodule";
}

# devices are ignored, but preserved in comments
/^[mM]/ {
	print "// " $0;
}

/^[xX]/ {
	# search backward from parameters to find type
	for (t=NF; t>0; --t) {
	if (!match($(t), "=")) {
		break;
		# t points to the type
	}
	}
	# TODO: not bothering with #() parameters yet
	inst = $(t) " " substr($1,2) " (";
	for (i=2; i<t; ++i) {
		if (!length(ports[$(i)]) && !length(wires[$(i)])) {
			wires[$(i)] = 1;
			print "wire " $(i) ";";
		}
		if (i!=2) {
			inst = inst ", ";
		}
		inst = inst $(i);
	}
	inst = inst ");";
	# save these in a buffer, print them all at the end
	insts[xc] = inst;
	++xc;
}

