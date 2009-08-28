#!/usr/bin/awk -f
#	$Id: prsimlog_to_evcd_ports.awk,v 1.1 2009/08/28 22:41:50 fang Exp $
#	"prsimlog_to_evcd_ports.awk"
# produces a flattened evcd trace file port list from prsim watchall logs
#	all signals will be scalarized!
# input: any dump of prsim events
# TODO: also export vcd as an option

function error_usage() {
print "usage: prsimloc_to_evcd_ports.awk";
print "optional arguments:";
print "\tformat : [evcd|vcd]";
print "input file/stream resemble a dump from prsim using watchall.";
}

BEGIN {
	count = 0;
}

# every line
{
if (NF >= 4 && $3 == ":") {
	if (!length(port_map[$2])) {
		port_map[$2] = count;
		port_vec[count] = $2;
		++count;
	}
}
# else just ignore
}

END {
for (i=0; i<count; ++i) {
	print "$var\tport\t1\t<" i "\t" port_vec[i] "\t$end";
}
}

