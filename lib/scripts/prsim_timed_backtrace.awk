#!/usr/bin/awk -f
# "prsim_timed_backtrace.awk"
#	$Id: prsim_timed_backtrace.awk,v 1.4 2011/05/07 03:43:40 fang Exp $
# follows a causality chain of events from a prsim watchall dump
# The algorithm used is precisely that of critical path finding. 

# recommended use: (where $< is the log file with 'watchall')
# tac $< | awk -f <this_script>
# optional prefilter:
# grep "\t[0-9][0-9]*.* : [01X]" $< | tac | awk -f <this_script>

function error_usage() {
print "usage: prsim_timed_backtrace.awk -v signal=... -v value=...";
print "\tsignal : the name of the signal to start searching";
print "\tvalue : the value of the signal to start searching";
print "optional arguments:";
print "\tstart : the latest time to start tracing";
print "input file/stream resemble a dump from prsim using watchall, ";
print "but in reverse-order (e.g. using tac), and may be filtered.";
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
#	prev_edge = make_signal_regex(signal, value);
#	print "prev_edge regex: " prev_edge;
	time = 0;	# or rather, +inf
}

# takes a string and returns the exact match regular expression for it
# function exact_regex(str) {
#	gsub("\\[", "\\[", str);
#	gsub("\\]", "\\]", str);
#	gsub("\\.", "\\.", str);
#	return str;
# }

# function make_signal_regex(sig, val) {
#	return "\\t[0-9][0-9]*.*\\t" exact_regex(sig) " : " val;
# }

# every line
{
if (NF < 4) {
	print "Malformed trace line, on line: " NR;
	print;
	exit(1);
}
# check time monotonicity
	newtime = strtonum($1);
if (time > 0 && newtime > time) {
	print "Error: time is not monotonically decreasing!  got: " newtime " vs. " time;
	print "Did you forget to reverse the stream (tac), perhaps?";
	exit(1);
}
	time = newtime;
if (start == 0 || time <= start) {
# can we just use equality comparison, strip out known sugar?
# if (match($0, prev_edge))
if ($2 == signal && $4 == value) # expect format: "time $2 : $4 ..."
{
	print;
if (match($0, "\\[by.*:=[01X]\\]")) {
	# hope this format never changes!
	signal = substr($0, RSTART, RLENGTH);
	value = signal;
	gsub("^.*\\[by ", "", signal);
	gsub(":=.*$", "", signal);
	gsub("^.*:=", "", value);
	gsub("\\].*$", "", value);
#	prev_edge = make_signal_regex(signal, value);
#	print "next search for " signal " : " value;
#	print "prev_edge regex: " prev_edge;
} else {
	print "No cause event to track.";
#	prev_edge = make_signal_regex("@@@@", "@");	# bogus
	exit (0);		# quickly exit
}
} # end if match edge
} # end if start time
}

