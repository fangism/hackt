#!/usr/bin/awk -f
# "scripts/tally_tests.awk"

BEGIN {
	fail = 0;
	all = 0;
}

/^All [0-9]+ tests/ {
	all += $2;
}

/^[0-9]+ of [0-9]+ tests/ {
	fail += $1;
	all += $3;
}

END {
	print "Total of " fail " failures out of " all " tests.";
}
