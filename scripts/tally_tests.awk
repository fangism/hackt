#!/usr/bin/awk -f
# "scripts/tally_tests.awk"
#	$Id: tally_tests.awk,v 1.1.16.1 2005/11/06 21:54:28 fang Exp $
# counting failures in test suite, used on resulting "check.log" file
#	produced by make check.log (in any directory)

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
	if (fail)
		exit 1;
}
