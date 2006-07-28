#!/usr/bin/awk -f
# "scripts/tally_tests.awk"
#	$Id: tally_tests.awk,v 1.3 2006/07/28 01:03:14 fang Exp $
# counting failures in test suite, used on resulting "check.log" file
#	produced by make check.log (in any directory)

BEGIN {
	fail = 0;
	all = 0;
	skip = 0;
}

/^All [0-9]+ tests/ {
	all += $2;
}

/^[0-9]+ of [0-9]+ tests/ {
	fail += $1;
	all += $3;
}

/[0-9]+ tests were not run/ {
	skip += substr($1, 2, length($1) -1);
}

END {
	print "Total of " fail " failures out of " all " tests.";
	print "Total of " skip " tests skipped.";
	if (fail)
		exit 1;
}
