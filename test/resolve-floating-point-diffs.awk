#!/usr/bin/awk -f
# "resolve-floating-point-diffs.awk"
# reads a unified diff and attempts to resolve the numerical 
# differences between floating-point values withing a certain rel_tol.
# TODO: some fields should be relative, others should be absolute diffs

BEGIN {
	# relative tolerance
	if (length(rel_tol)) {
		rel_tol = strtonum(rel_tol);
	} else {
		rel_tol = 0.001;
	}
	if (length(abs_tol)) {
		abs_tol = strtonum(abs_tol);
	} else {
		abs_tol = 0.001;
	}
	hunk_count = 0;
	float_regex = "[\\+\\-]?[0-9]+(\\.[0-9]+)?([eE][\\+\\-]?[0-9]+)?";
}

function reset_hunk() {
	for (l in old_lines)
		delete old_lines[l];
	num_old_lines = 0;
	for (l in new_lines)
		delete new_lines[l];
	num_new_lines = 0;
}

# returns a space-delimited list of extracted numbers
function extract_real_numbers(str,
	# local vars
	ret) {
	ret = "";
	while (match(str, float_regex)) {
		ret = ret " " substr(str, RSTART, RLENGTH);
		str = substr(str, RSTART +RLENGTH);
	}
#	print ret;
	return ret;
}

function fabs(x) {
	return (x < 0.0) ? -x : x;
}

function absolute_change(n1, n2) {
	n1 = strtonum(n1);
	n2 = strtonum(n2);
	return fabs(n2 -n1);
}

# always returns a positive relative change
function relative_change(n1, n2,
	# local vars
	ret) {
	if (n1 == n2) {
		return 0.0;
	}
	# caution: divide by 0, just ignore
	if (n1 == 0.0) {
		return 0.0;
	}
	ret = fabs(absolute_change(n1, n2)/n1);
	return ret;
}

# return 0 for unknown
# return 1 for coordinates: [ID]: <3D-pos> <3D-vel> <3D-acc>
# return 2 for energy
# return for iteration report
function detect_line_format(str) {
	if (match(str, "\\[[0-9]+\\]:")) {
		return 1;
	}
	if (match(str, "T=")) {
		return 2;
	}
	if (match(str, "ran .* from time")) {
		return 3;
	}
	return 0;
}

# all local vars
function process_hunk_diffs(ntoks_old, ntoks_new, i, j, dr, da) {
# if (num_new_lines == num_old_lines) {
	i = 0;
	for ( ; i<num_old_lines; ++i) {
		ntoks_old = split(old_lines[i], old_nums);
		ntoks_new = split(new_lines[i], new_nums);
		if (ntoks_old != ntoks_new) {
			print "Error in " FILENAME;
			print "Number of number tokens mismatch! near line: " FNR;
			print "got:\t" old_lines[i];
			print "vs.:\t" new_lines[i];
			# exit (1);
		}
		j = 1;
		for ( ; j<=ntoks_old; ++j) {
			dr = relative_change(old_nums[j], new_nums[j]);
			da = absolute_change(old_nums[j], new_nums[j]);
			if (dr > rel_tol && da > abs_tol) {
			print FILENAME ", hunk: " hunk_count ", line: " i \
				", token: " j ", old: " old_nums[j] \
				", new: " new_nums[j] ", abs-diff: " da \
				", rel-diff: " dr;
			}
		}
	}
	reset_hunk();
# }
}

/^@@/ {
	if (num_old_lines) {
		process_hunk_diffs();
	}
	reset_hunk();
	++hunk_count;
}

/^-/ {
if ($1 != "---") {
	old_lines[num_old_lines] = extract_real_numbers($0);
	++num_old_lines;
}
}

/^+/ {
if ($1 != "+++") {
	new_lines[num_new_lines] = extract_real_numbers($0);
	++num_new_lines;
}
}

END {
	process_hunk_diffs();
}

