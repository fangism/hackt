#!/usr/bin/awk -f
# "promote_header_deps.awk"

# Description:
#	Takes a make-dependence file produced by gcc -MD
#	and promotes the .h and .tcc dependencies to their corresponding
#	checked header target, like .hchk and .tccchk

# Usage:
#	-v srcdir=<path> -- the path to srcdir, may be left blank

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
			# line-continuation
			print " \\";
		} else if (!index(toks[i], current_target)) {
			# need to point to build directory, not srcdir!
			printf(" %s", srcdir_to_builddir( \
				promote_header_dep(toks[i])));
		} else if (!match(toks[i], "\\.cc$")) {
			# ignore .cc (non-header source) files
			printf(" %s", toks[i]);
		}
		# else silent
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

# Since gcc -MD resolves header dependencies to their srcdir paths, 
# we have to undo that effect, by redirecting it to look in the
# builddir for generated .hchk and .tccchk files.  
# To do this, for every "../" in the path string, we strip it off
# as well as the first non "../" part of the path, EXCEPT the first one.  
# Repeat until path doesn't start with "../".
# For Windows, which uses "..\" I'll have to punt and make this script
# configure-dependent later.  
function srcdir_to_builddir(f, 
	# local variable
	i, stripcount) {
	# assuming "../" doesn't appear in the middle...
	stripcount = gsub("\\.\\.\\/", "", f);
	if (stripcount > 0) {
		# one LESS than the number stripped
		for (i=0; i < stripcount-1; i++) {
			sub("^[^/]*\\/", "", f);
		}
	}
	return f;
}

END {
	print "";
	# print one phony target for the top-level header file
	if (length(srcdir))
		printf(srcdir "/");
	# else leave blank
	print demote_header_dep(current_target) ":";
}

