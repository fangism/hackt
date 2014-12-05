#!/usr/bin/awk -f
# "spice_to_dot.awk"
# create an undirected graph
# pass to graphviz: dot, neato, etc...
# TODO:
#	* this currently works on one subcircuit at a time
#	detect which ports are inputs and which are outputs
#	assign top/bottom rank to Vdd, GND? (attribute/supply)

# Tips: files with line continuations should be 'joined' using
# spice_line_join.awk.

BEGIN {
	# overrideable
	if (!length(directed)) {
		directed = 1;	# default: directed
	}
	# directed: dot, undirected: neato
	edge = (directed ? " -> " : " -- ");
}

# TODO: make device type pattern overrideable
function is_nfet(t) {
	return match(t, "^[nN]");
}

function is_pfet(t) {
	return match(t, "^[pP]");
}

/^\.subckt/ {
	ntoks = split($0, toks);
	# maybe undirected?
	print "# subcircuit: " $2;
	print (directed ? "digraph" : "graph") " G {";
	print "concentrate = false;";
	print "nodesep = 0.5;";
	print "node [shape=ellipse];";	# or point, plaintext
#	print "edge [constraint=false];";
	for (i=3; i<=ntoks; ++i) {
		# mark ports differently
#		print "\"" toks[i] "\"\t[shape=ellipse,style=filled];";
		ports[toks[i]] = 1;
	}
}

/^\.ends/ {
	for (w in wires) {
		print "\"" w "\"\t[label=\"" w "\"];";
		if (ports[w]) {
			print "\"" w "\"\t[shape=" (fanin[w] ? "triangle" : "invtriangle") "];";
		}
	}
#	for (v in vdd) {
#		print "\"" v "\"\t[rank=max];";
#	}
#	for (g in gnd) {
#		print "\"" g "\"\t[rank=min];";
#	}
	print "}";
}

function reset_graph() {
	for (w in wires) {
		delete wires[w];
	}
	for (p in ports) {
		delete ports[p];
	}
	for (f in fanin) {
		delete fanin[f];
	}
	for (v in vdd) {
		delete vdd[v];
	}
	for (g in gnd) {
		delete gnd[g];
	}
}

/^[mM]/ {
# parse transistor
	dev_name = $1;
	qdev_name = "\"" dev_name "\"";
	source = $2;
	gate = $3;
	drain = $4;
	bulk = $5;
	wires[source] = 1;
	wires[gate] = 1;
	wires[drain] = 1;
	wires[bulk] = 1;
	# quoted names
	qsource = "\"" source "\"";
	qgate = "\"" gate "\"";
	qdrain = "\"" drain "\"";
	qbulk = "\"" bulk "\"";
	type = $6;
	# ignore other parameters
	# $2,$3,$4,$5 = s,g,d,b
	if (is_nfet(type)) {
		print qdev_name "\t[shape=record,label=\"{<d>d|<g>" dev_name "|<s>s}\",fontcolor=green];";
		# try headport/tailport directions?
		print qdev_name ":s" edge qsource ";";
		print qgate edge qdev_name ":g\t[constraint=false];"; # --
		print qdrain edge qdev_name ":d;";
		fanin[drain] = 1;
		gnd[bulk] = 1;
	}
	else if (is_pfet(type)) {
		print qdev_name "\t[shape=record,label=\"{<s>s|<g>" dev_name "|<d>d}\",fontcolor=red];";
		print qsource edge qdev_name ":s;";
		print qgate edge qdev_name ":g\t[constraint=false]"; # --
		print qdev_name ":d" edge qdrain ";";
		fanin[drain] = 1;
		vdd[bulk] = 1;
	} else {
		print "Error: unknown FET type, " type ".";
		exit 1;
	}
}

function generic_two_terminal() {
	dev_name = $1;
	qdev_name = "\"" dev_name "\"";
	t1 = $2;
	t2 = $3;
	qt1 = "\"" t1 "\"";
	qt2 = "\"" t2 "\"";
	print qdev_name "\t[shape=box,label=" qdev_name "];";
	print qt1 edge qdev_name "\t[constraint=false];";
	print qdev_name edge qt2 "\t[constraint=false];";
}

/^[rR]/ {
# parse resistor
	generic_two_terminal();
}

/^[cC]/ {
# parse capacitor
	generic_two_terminal();
}

/^[xX]/ {
# parse instance
# TODO:
	ntoks = split($0, toks);
	dev_name = $1;
	qdev_name = "\"" dev_name "\"";
	# TODO: construct record based on subckt definition's ports
	# TODO: direction of edges based on port direction
	print qdev_name "\t[shape=box,peripheries=2,label=" qdev_name "];";
	# last token is subckt type
	for (i=2; i<ntoks; ++i) {
		n = toks[i];
		qn = "\"" n "\"";
		print qn edge qdev_name "\t[constraint=false];";
	}
}

