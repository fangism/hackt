#!/usr/bin/awk -f
# "prsim_timed_fwdtrace.awk"
#	$Id: prsim_timed_fwdtrace.awk,v 1.1 2011/05/07 03:43:41 fang Exp $
# follows a causality chain of events from a prsim watchall dump
# This algorithm finds forward-flowing critical paths from the root transition
# The chase is on!

# recommended use: (where $< is the log file with 'watchall')
# awk -f <this_script> $<
# optional prefilter:
# grep "\t[0-9][0-9]*.* : [01X]" $< | tac | awk -f <this_script>

# TODO: depth limit

function error_usage() {
print "usage: prsim_timed_fwdtrace.awk -v signal=... -v value=...";
print "\tsignal : the name of the signal to start searching";
print "\tvalue : the value of the signal to start searching";
print "optional arguments:";
print "\tstart : the earliest time to start tracing";
print "\tindent : indent string (default: \"  \"";
print "input file/stream resembles a dump from prsim using watchall";
print "output is a tree-formatted chain of causality from the root";
}

BEGIN {
	err = 0;
	if (!length(signal)) err = 1;
	if (!length(value)) err = 1;
	if (err) {
		error_usage();
		exit(1);
	}
	if (length(start)) {
		start = strtonum(start);
	} else {
		start = 0;
	}
	if (!length(indent)) {
		indent = "  ";
	}
	time = 0;	# or rather, +inf
	# initialize root transition
	found_root = 0;
}

# check every line for root cause
{
if (!found_root) {
if ($2 == signal && $4 == value) {
	worklist[signal ":" value] = 1;
	str = $0;
	gsub("^[ \t]*", "", str);
	print str " (root)";
	found_root = 1;
}
}
}

# every line with a critical cause
/\[by.*:=[01X]\]/ {
if (NF < 4) {
	print "Malformed trace line, on line: " NR;
	print;
	exit(1);
}
# check time monotonicity
	newtime = strtonum($1);
if (time > 0 && newtime < time) {
	print "Error: time is not monotonically increasing!  got: " newtime " vs. " time;
	exit(1);
}
	time = newtime;
if (start == 0 || time >= start) {
# can we just use equality comparison, strip out known sugar?
if (match($0, "\\[by.*:=[01X]\\]")) {
	# hope this format never changes!
	by_signal = substr($0, RSTART, RLENGTH);
	by_value = by_signal;
	gsub("^.*\\[by ", "", by_signal);
	gsub(":=.*$", "", by_signal);
	gsub("^.*:=", "", by_value);
	gsub("\\].*$", "", by_value);
	by_transition = by_signal ":" by_value;
	probe = worklist[by_transition];
	if (length(probe)) {
		new_transition = $2 ":" $4;
		# add to critical set
		worklist[new_transition] = probe +1;
		printf $1 "\t";		# time
		for (i=0; i<probe; ++i) {
			printf indent;		# indent string
		}
		print new_transition "\t[by " by_transition "]";
	}
}
} # end if start time
}

