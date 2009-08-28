#!/usr/bin/awk -f
#	$Id: prsimlog_to_evcd.awk,v 1.1 2009/08/28 22:41:49 fang Exp $
#	"prsimlog_to_evcd.awk"
# produces a flattened evcd trace file from prsim watchall logs
#	all signals will be scalarized!
# input: a dump of prsim events
# requires: separate pass to emit port declarations
# TODO: also export vcd as an option

function error_usage() {
print "usage: prsimlog_to_evcd.awk -v port_file=...";
print "\tport_file : evcd header file for all signals in the trace";
print "optional arguments:";
print "\tformat : [evcd|vcd]";
print "\tstart : the latest time to start tracing";
print "input file/stream resemble a dump from prsim using watchall.";
}

BEGIN {
# start by reading in port map
	if (!length(port_file)) {
		error_usage();
		exit (1);
	}
	load_port_map(port_file);
	print "$enddefinitions $end";
	time = 0;
	print "#" time;
	dumpports();
	print "#" time;
}

function load_port_map(f) {
	print "$scope module PRSIM $end";
	while (getline < f) {
	if (NF == 6 && $1 == "$var" && $6 == "$end") {
		port_map[$5] = $4;
		print;
	}
	}
	print "$upscope $end";
}

# start with all values X
function dumpports() {
	print "$dumpports";
	for (p in port_map) {
		print "pF\t0\t0\t" port_map[p];
	}
	print "$end";
}

# every line
{
if (NF >= 4 && $3 == ":") {
	newtime = strtonum($1);
if (newtime > time) {
	print "#" newtime;
}
	time = newtime;
	if ($4 == "0") {
		val = "D";
		dn = 6;
		up = 0;
	} else if ($4 == "1") {
		val = "U";
		dn = 0;
		up = 6;
	} else {
		# treat X as interference or floating?
		val = "N";
		dn = 6;
		up = 6;
	}
	print "p" val "\t" dn "\t" up "\t" port_map[$2];
}
# else just ignore
}

END {
	print "$vcdclose #" time " $end";
}

