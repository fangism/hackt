#!/usr/bin/awk -f
# "promote_header_deps.awk"

# Description:
#	Takes a make-dependence file produced by gcc -MD
#	and promotes the .h and .tcc dependencies to their corresponding
#	checked header target, like .hchk and .tccchk

BEGIN {	current_target = ""; }

{
	n = split($0, toks);
	for (i=1; i<=n; i++) {
		# print "toks[" i "] = " toks[i];
		if (match(toks[i], ":$")) {
			# only applies to first non-phony dependency
			if (!length(current_target)) {
				# print "FOUND CHECK TARGET!";
				current_target = toks[i];
				# strip off colon and chk part of file extension
				sub("chk:$", "", current_target);
				# print "target = " current_target;
				printf("%s", toks[i]);
			}
			# else ignores all other lone phony targets
		} else if (toks[i] == "\\") {
			print " \\";
		} else if (toks[i] != current_target) {
			printf(" %s", promote_header_dep(toks[i]));
		} else {
			printf(" %s", toks[i]);
		}
	}
	# if (!n) print "";
}

# transforms * .h, .hh, .tcc : appends "chk"
function promote_header_dep(f) {
	if (match(f, "\\.h$") || match(f, "\\.tcc$") || match(f, "\\.hh$"))
		return f "chk";
	else	return f;
}

function demote_header_dep(f) {
	sub("chk$", "", f);
	return f;
}

END {
	print "";
	# print one phony target for the top-level header file
	print demote_header_dep(current_target) ":";
}

