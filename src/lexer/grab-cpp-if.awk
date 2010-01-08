#!/usr/bin/awk -f

BEGIN {
	if (!length(marker)) {
		print "Must define marker string (-v marker=...)"
		exit 1;
	}
	if_regex = "#if";
	endif_regex = "#endif";
	start_regex = if_regex ".*" marker;
	level = 0;
	start_level = 666;
}

{
	if (match($0, if_regex)) {
		++level;
		if (match($0, start_regex)) {
			start_level = level;
		}
	}
	if (level >= start_level) {
		print;
	} else {
		start_level = 999;
	}
	if (match($0, endif_regex)) {
		--level;
	}
}

