#!/usr/bin/awk -f
#	$Id: scantexdepend.awk,v 1.9.2.1 2007/08/23 21:36:05 fang Exp $
# "scantexdepend.awk"
# Scans [pdf][la]tex .log files for include dependencies.  
# usage: awk -f scantexdepend.awk [-v targets="..."] yourfile.log
# options: -v ignore_extensions="aux bbl ind lof lot out toc"

BEGIN {
	# optional: space-delimited targets
	if (length(targets)) {
		printf("%s:", targets);
	}
	# set of extensions to ignore
	if (length(ignore_extensions)) {
		ntoks = split(ignore_extensions, toks);
		for (i=1; i<=ntoks; ++i) {
			ignore_ext[toks[i]] = 1;
		}
	}
	hold = "";
}

# valid readable file? (not a directory)
# reject invalid characters (those that have special shell meanings)
function valid_file(str) {
	return !match(str, "[][{}<>;*&?()]") &&
		!system("test -f " str " && test -r " str);
}

# the file extension string (excluding '.')
function extract_extension(str) {
	if (match(str, "\\.[^./]*$")) {
		return substr(str, RSTART+1, RLENGTH-1);
	} else {
		return "";
	}
}

# file passes pattern filters
function matched_file(f) {
	# print "\nEXT: " extract_extension(f);
	return match(f, "[A-Za-z0-9_-]$") &&
		!ignore_ext[extract_extension(f)];
}

# /\([/\.]/
{
	# print "GOT: " $0;
	buffer = $0;
	len = length(hold);
	if (len) {
		# print "\nholding: " hold;
		hold = hold $0;		# concatenate
		# try substrings of concatenated string until file found
		# print "cat: " hold;
		max = length(hold);
		while (len <= max) {
			try_file = substr(hold, a, len);
			# print "try: " try_file;
			# careful, valid_file clobbers $0
			if (valid_file(try_file)) {
				if (matched_file(try_file)) {
					printf("%s", " \\\n\t" try_file);
				}
				# possibly eat up part of $0
				break;
			}
			++len;
		}
	}
	hold = "";	# always clear it out
	# the normal file-name search
	$0 = buffer;
	while (match($0,"\\([/\\.]")) {
		# eat up everything up to and including the open-parenthesis
		sub("^[^(]*\\(", "", $0);
		# print "\n# HAVE: " $0;
		ntoks = split($0, toks);
		dep = toks[1];
		# eat closing paren if attached to first token
		gsub("[)]+", "", dep);
		# print "\n# DEP: " dep;
		# File name must end with a 'normal' character.
		# Suppress auxiliary dependencies, because they are cleaned
		# and we do not write suffix rules for them (yet).

		# verify validity of file
		if (matched_file(dep) && valid_file(dep)) {
			printf("%s", " \\\n\t" dep);
		} else {
			# uh oh, filename was snipped!
			# printf("%s_NOT_FOUND", " \\\n\t" dep);
			hold = dep;
		}
		# print "\n# LEFT: " $0;
	}
}

END {
	# end file with newline
	print "";
	print "";
}

