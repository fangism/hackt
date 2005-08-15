#!/usr/bin/awk -f
# "test/analyze-dump-diff.awk"
#	$Id: analyze-dump-diff.awk,v 1.1.4.2 2005/08/15 21:12:28 fang Exp $

# Utility for automatically analyzing the differences produced by the
# HACKT test suite.  

# input: format is the unified-diff format, the result of diff -u.
# -v quiet={0,1,2}
#	only print message if there are non-header differences

BEGIN {
	debug = 0;
	first_file = 1;
	# quiet = 0;
	# counters
	reset_per_file_counters();
}

/^@@/ {
	if (debug) print "where:" $0;
}

/^---/ {
	if (debug) print "old:  " $2;
	if (first_file) {
		first_file = 0;
	} else {
		summary();
	}
	reset_per_file_counters();
	if (quiet < 2)
		print "Inspecting " FILENAME;
}

/^\+\+\+/ {
	if (debug) print "new:  " $2;
}

# removed lines
# note: this won't work on lines that naturally start with -
/^-[^-]/ {
	if (debug) print "was:  " $0;
	removed++;
	if (!is_header_entry())
		nonheader_removed++;
}

# added lines
# note: this won't work on lines that naturally start with +
/^\+[^+]/ {
	if (debug) print "is:   " $0;
	added++;
	if (!is_header_entry())
		nonheader_added++;
}

# everything else is just context
!/^-/ && !/^\+/ && !/^@@/ {
	if (debug) print "same: " $0;
}

# utility functions ###########################################################

function reset_per_file_counters() {
	removed = 0;
	added = 0;
	nonheader_removed = 0;
	nonheader_added = 0;
}

# expects to have just read a line from stdin into $0
function is_header_entry() {
	# remember to include the +/- in the count
	# normally 7 fields +1
	if (NF == 8) {
		# make sure this is consistent with the output of
		# "test/POM-header-filter.awk"
		# maybe use match instead of ==
		if ($3 == "##ADDR##" && $6 == "#HEAD#" && $7 == "#TAIL#")
			return 1;
		else	return 0;
	} else {
		return 0;
	}
}

function summary() {
	if (!quiet || nonheader_removed || nonheader_added) {
		print "-" removed " lines (" nonheader_removed " non-header)";
		print "+" added " lines (" nonheader_added " non-header)";
	}
	if (!nonheader_removed && !nonheader_added) {
		print FILENAME ": only header differences found: OK";
	} else {
		print FILENAME ": UH OH, non-header differences found!";
	}
}

END {
	summary();
}

