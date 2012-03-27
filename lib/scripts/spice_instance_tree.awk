#!/usr/bin/awk -f
# "spice_instance_tree.awk"
# draws top-down hierarchy figure
# target format: twopi

# read entire hierarchical netlist first
# with each subcircuit generate a local graph
# in the end, for each requested top-level
# print complete subgraph recursively for every subinstance

# options:
# -v subckt=TYPE,... list of types to output graphs for
#	if omitted, assumes last subckt
# -v depth=N limit recursion depth (default 100, practically unlimited)
# -v wirelabels=[0|1] to enable or disable edge labels with signals
# -v cluster=[0|1] to enable subgraph clustering

BEGIN {
#	debug = 1;
	target = "";
	if (!length(depth)) {
		depth = 100;
	}
	if (!length(cluster)) {
		cluster = 0;
	}
}

function begin_subckt() {
	target = $2;
	subs = "";
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

function end_subckt() {
	subinstances[target] = subs;
	last_target = target;
	target = "";
}

/^\.ends/ {
	end_subckt();
}
/^\.ENDS/ {
	end_subckt();
}

function member_inst(tp, inst) {
	return tp ":" inst;
}

function process_subinstance(tgt,
	# local vars
	subinst, toks, ntoks, i, ports) {
# amend local subgraph
	subinst = member_inst(tgt, $1);
	# accumulate list of subinstances (global subs)
	subs = subs " " $1;
	ports = "";
	ntoks = split($0, toks);
	# first token is instance name, last token is type
	for (i=2; i<ntoks; ++i) {
		ports = ports " " toks[i];
	}
	wires[subinst] = ports;
	# assuming no parameters
	type[subinst] = toks[ntoks];		# the type
}

/^[xX]/ {
if (length(target)) {
# amend local subgraph
	process_subinstance(target);
} else {
	# else is top-level instance
	process_subinstance("<TOP>");
}
}

# ignore transistors
# /^[mM]/ { }

# mangle
function dot_node_name(str,
	# local var
	ret) {
	ret = str;
	gsub("[/<>\\[\\]]", "__", ret);
	return ret;
}

function parent_name(str) {
	if (length(str))
		return str;
	else	return "<TOP>";
}

function child_name(parent, str) {
	if (length(parent))
		return parent "/" str;
	else	return str;
}

function edge_string(p, c, l,
	# local vars
	ret, nfirst, dnn) {
	nfirst = 1;
	ret = dot_node_name(p) " -> ";
	dnn = dot_node_name(c);
	ret = ret dnn;
	ret = ret " [";
	if (wirelabels) {
		ret = ret "label=\"" l "\"";
		nfirst = 0;
	}
if (0) {
	# not quite working yet, as leafs have empty clusters
	if (cluster) {
		if (!nfirst)
			ret = ret "; ";
		ret = ret "lhead=cluster_" dnn;
	}
}
	ret = ret "];";
	return ret;
}

function node_string(n, tp) {
	return dot_node_name(n) " [label=\"" n "\\n" tp "\"];";
}

function root_node_string(n, tp) {
	return dot_node_name(n) " [label=\"" n "\\n" tp "\", peripheries=2, style=filled, shape=ellipse];";
}

# wrap long lines into multi-line, input delimited by spaces
function wrap_line(str, lim,
	# local vars
	i, toks, ntoks, len, ret) {
	len = 0;
	ret = "";
	ntoks = split(str, toks);
	for (i=1; i<=ntoks; ++i) {
		ret = ret " " toks[i];
		len += length(toks[i]) +1;
		if (len >= lim && i!=ntoks) {
			len = 0;
			ret = ret "\\n";
		}
	}
	return ret;
}

# choose a wrapping limit based on total line length
# decent heuristic: 3*sqrt(length), minimum of 8
function auto_wrap_line(str, 
	# local vars
	len) {
	len = 8*sqrt(len);
	if (len < 14) { len = 14; }
	return wrap_line(str, len);
}

# \param type the type of circuit to expand
# \param name the instance name (may be blank), prefix will grow
function emit_subgraph(tp, name, d,
	# local vars
	pname, i, j, ntoks, toks, subinst, wirelist, w, nw, ctype) {
if (d) {
	if (cluster) {
		print "subgraph cluster_" dot_node_name(name) " {";
	}
	pname = parent_name(name);
	ntoks = split(subinstances[tp], toks);
	for (i=1; i<=ntoks; ++i) {
		# draw edges to that node for every wire
		subinst = member_inst(tp, toks[i]);
		wirelist = wires[subinst];
#		nw = split(wirelist, w);
#		for (j=1; j<=nw; ++j) {
#			w[j]
#		}
		child = child_name(name, toks[i]);
		ctype = type[subinst];
		print node_string(child, ctype);
		print edge_string(pname, child, auto_wrap_line(wirelist));
		emit_subgraph(ctype, child, d-1);	# recursion
	}
	if (cluster) {
		print "}";
	}
}
}

function emit_graph(tp,
	# local vars
	p) {
	# wrap graph in prolog and epilogue
	print "# subckt: " tp;
	print "# vi: syntax=dot";
	print "digraph G {";
	print "rankdir=LR;";
	print "ranksep=1;";
if (cluster) {
	print "compound=true;";
}
	print "overlap=false;";
#	print "splines=true;";
	print "node [shape=box];";
	print "edge [fontsize=9];";
	p = parent_name("");
	print "root=" dot_node_name(p) ";";
	print root_node_string(p, tp);
	emit_subgraph(tp, "", depth);
	print "}";
	print "# ends: " tp;
}

END {
	if (!length(subckt)) {
		subckt = last_target;
	}
	ntoks = split(subckt, toks, ",");
	for (i=1; i<=ntoks; ++i) {
		emit_graph(toks[i]);
	}
}
