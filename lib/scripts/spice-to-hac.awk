#!/usr/bin/awk -f
# "spice-to-hac.awk"
# converts hierarchical spice netlist into HAC definitions
#	where primitives are represented as processes as well.
# input must not contain line-continuations, i.e. already filtered through
#	spice_line_join.awk
# options:
#	-v keep_card_prefix=[01]
#		keep the leading [XRCL] in instance names (default 1)
#	-v passive_device_values=[01]
#		keep values of passive devices (R,C,L), (default: 1)

# TODO: support nested subcircuits
# TODO: support device parameters

BEGIN {
	if (length(keep_card_prefix)) {
		keep_card_prefix = strtonum(keep_card_prefix);
	} else {
		keep_card_prefix = 1;
	}
	if (keep_card_prefix) {
		inst_name_start = 1;
	} else {
		inst_name_start = 2;
	}
	if (length(passive_device_values)) {
		passive_device_values = strtonum(passive_device_values);
	} else {
		passive_device_values = 1;
	}
}

# note port signals as already having been declared
function record_port(str) {
	have_decl[str] = 1;
}
function auto_declare(str) {
	if (!have_decl[str]) {
		print "bool " auto_escape(str) ";";
		have_decl[str] = 1;
	}
}

# automatically escape identifiers that need it
function auto_escape(str) {
	if (match(str, "[^0-9a-zA-Z_]")) {
		return "\\" str " ";
	} else {
		return str;
	}
}

# converts scientific number suffixes
function normalize_sci_val(str) {
	return str;
}

/^\.subckt/ || /^\.SUBCKT/ {
	split($0, toks);
	print "defproc " auto_escape($2) " (bool";
	for (i=3; i < NF; ++i) {
		record_port(toks[i]);
		print "\t" auto_escape(toks[i]) ",";
	}
	record_port(toks[NF]);
	print "\t" auto_escape(toks[NF])
	print ") {";
}

/^\.ends/ || /^\.ENDS/ {
	print "}\n";
	# reset declarations
	for (f in have_decl) {
		delete have_decl[f];
	}
}


/^.connect/ || /^\.CONNECT/ {
	print auto_escape($2) " = " auto_escape($3) ";";
}

/^[xX]/ {
	# don't expect any parameters
	split($0, toks);
	actuals = "";
	for (i=2; i<NF-1; ++i) {
		auto_declare(toks[i]);
		actuals = actuals auto_escape(toks[i]) ", ";
	}
	auto_declare(toks[NF-1]);
	actuals = actuals auto_escape(toks[NF-1]);
	print auto_escape(toks[NF]) " " \
		auto_escape(substr($1,inst_name_start)) "(" actuals ");";
}

# for 2-terminal devices
function default_print_device(type,
	# local vars
	param) {
	param = "";
	if (passive_device_values) {
		param = "<" normalize_sci_val($4) ">";
	}
	auto_declare($2);
	auto_declare($3);
	print type " " auto_escape(substr($1,inst_name_start)) \
		param "(" auto_escape($2) ", " auto_escape($3) ");";
}

# C1 net1 net2 val
/^[cC]/ {
	default_print_device("CAPACITOR");
}

# TODO: handle passive device models (with subtypes)
/^[rR]/ {
	default_print_device("RESISTOR");
}

/^[lL]/ {
	default_print_device("INDUCTOR");
}

/^[dD]/ {
	default_print_device("DIODE");
}

# transistors
/^[mM]/ {
	split($0, toks);
	for (i=2; i<6; ++i) {
		auto_declare(toks[i]);
	}
	actuals = auto_escape($2) ", " auto_escape($3) ", " \
		auto_escape($4) ", " auto_escape($5);
	print "TRANSISTOR_" $6 " " auto_escape(substr($1,inst_name_start)) \
		"(" actuals ");";
	# for now ignore parameters
}

