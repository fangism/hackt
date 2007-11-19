#!/usr/bin/awk -f
# "promote_header_deps.awk"
#	$Id: promote_header_deps.awk,v 1.7 2007/11/19 07:49:18 fang Exp $

# Description:
#	Takes a make-dependence file produced by gcc -MD
#	and promotes the .h and .tcc dependencies to their corresponding
#	checked header target, like .hchk and .tccchk

# Usage:
#	-v src=<file> -- the input source file, should NOT be transformed
#	-v srcdir=<paths> -- the path to srcdir, may be left blank
#		Multiple paths may be space-delimited, 
#		e.g., -v srcdir=".. ../.."
#		The first directory should be the primary srcdir,
#		i.e. the srcdir relative path from this working directory.

BEGIN {
	current_target = "";
	num_paths = split(srcdir, toks);
	for (i=1; i<=num_paths; ++i) {
		srcdirs[i] = toks[i];
	}
	if (!length(src)) {
		print "Error: -v src=<file> is required.";
		exit 1;
	}
	# what is assumed if no srcdirs are given? '.'?
	primary_srcdir = srcdirs[1];
}

# main:
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
			if (!match(toks[i], src)) {
			# omit src file
			printf(" %s", srcdir_to_builddir( \
				promote_header_dep(toks[i])));
			}
		} else if (!match(toks[i], "\\.cc$")) {
			# ignore .cc (non-header source) files
			printf(" %s", toks[i]);
		}
		# else silent
	}
	# if (!n) print "";
}

# \pre do not transform the source file name
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
# To accomplish this, we strip off the srcdir.
function srcdir_to_builddir(f, 
# local vars
	i) {
	for (i=1; i<=num_paths; ++i) {
		# exact substring match
		if (index(f, srcdirs[i] "/")) {
			return substr(f, length(srcdirs[i])+2);
		}
		# else continue to check next one
	}
	return f;
}

END {
	print "";
	# print one phony target for the top-level header file
	if (length(primary_srcdir) && primary_srcdir != ".")
		printf(primary_srcdir "/");
	# else leave blank
	print demote_header_dep(current_target) ":";
}

