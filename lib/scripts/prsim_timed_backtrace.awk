#!/usr/bin/awk -f
# "prsim_timed_backtrace.awk"
# follows a causality chain of events from a prsim watchall dump

# recommended use: (where $< is the log file)
# tac $< | awk -f <this_script>
# optional prefilter:
# grep "\t[0-9][0-9]*.* : [01X]" $< | tac | awk -f <this_script>

function error_usage() {
print "usage: prsim_timed_backtrace.awk -v signal=... -v value=...";
print "\tsignal : the name of the signal to start searching";
print "\tvalue : the value of the signal to start searching";
print "input file/stream resemble a dump from prsim using watchall, ";
print "but in reverse-order (e.g. using tac), and may be filtered.";
}

BEGIN {
	if (!length(signal)) error_usage();
	if (!length(value)) error_usage();
	prev_edge = make_signal_regex(signal, value);
#	print "prev_edge regex: " prev_edge;
}

# takes a string and returns the exact match regular expression for it
function exact_regex(str) {
	gsub("\\[", "\\[", str);
	gsub("\\]", "\\]", str);
	gsub("\\.", "\\.", str);
	return str;
}

function make_signal_regex(sig, val) {
	return "\\t[0-9][0-9]*.*\\t" exact_regex(sig) " : " val;
}

# every line
{
if (match($0, prev_edge)) {
	print;
if (match($0, "\\[by.*:=[01X]\\]")) {
	signal = substr($0, RSTART, RLENGTH);
	value = signal;
	gsub("^.*\\[by ", "", signal);
	gsub(":=.*$", "", signal);
	gsub("^.*:=", "", value);
	gsub("\\].*$", "", value);
	prev_edge = make_signal_regex(signal, value);
#	print "next search for " signal " : " value;
#	print "prev_edge regex: " prev_edge;
} else {
	print "No cause event to track.";
	prev_edge = make_signal_regex("@@@@", "@");	# bogus
}
}
}

