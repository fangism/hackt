#!/usr/bin/awk -f
# "promote_header_deps.awk"
#	$Id: promote_header_deps.awk,v 1.4 2006/02/21 04:48:47 fang Exp $

# Description:
#	Takes a make-dependence file produced by gcc -MD
#	and promotes the .h and .tcc dependencies to their corresponding
#	checked header target, like .hchk and .tccchk

# Usage:
#	-v srcdir=<path> -- the path to srcdir, may be left blank

BEGIN {
	current_target = "";
	strip_length = length(srcdir) +1;
	strip_dir = srcdir;
	strip_dir_regex = literal_string_to_regex(strip_dir);
#	print "# strip_dir = " strip_dir;
#	print "# strip_dir_regex = " strip_dir_regex;
}

# converts a literal string into a regular expression for that exact string
function literal_string_to_regex(str) {
	gsub("\\\\", "\\\\", str);
	gsub("\\.", "\\.", str);
# some of these are wacky, hope not to find these in any path name
#	gsub("\\[", "\\[", str);
#	gsub("\\]", "\\]", str);
#	gsub("\\^", "\\^", str);
#	gsub("\\$", "\\$", str);
#	gsub("\\?", "\\?", str);
#	gsub("\\&", "\\&", str);
	return str;
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
# To accomplish this, we strip off the srcdir.
function srcdir_to_builddir(f) {
#	if (match(f, "^" strip_dir_regex)) {
#		# just decapitate the $(srcdir)/ part of the path name
#		# same effect as substitution
#		i = length(f);
#		return substr(f, strip_length +1, i -strip_length +1);
#	} else	return f;
	sub("^" strip_dir_regex "/", "", f);
	return f;
}

END {
	print "";
	# print one phony target for the top-level header file
	if (length(srcdir) && srcdir != ".")
		printf(srcdir "/");
	# else leave blank
	print demote_header_dep(current_target) ":";
}

