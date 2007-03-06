#!/usr/bin/awk -f
#	$Id: scantexdepend.awk,v 1.6 2007/03/06 05:28:44 fang Exp $
# "scantexdepend.awk"
# Scans [pdf][la]tex .log files for include dependencies.  
# usage: awk -f scantexdepend.awk [-v targets="..."] yourfile.log

BEGIN {
	# optional: space-delimited targets
	if (length(targets)) {
		printf("%s:", targets);
	}
	hold = "";
}

# valid readable file? (not a directory)
function valid_file(str) {
	return !system("test -f " str " && test -r " str);
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
			if (valid_file(try_file) == 1) {
				printf("%s", " \\\n\t" try_file);
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
		if (match(dep, "[A-Za-z0-9_-]$") &&
			!match(dep, "\\.aux$") &&
			!match(dep, "\\.bbl$") &&
			!match(dep, "\\.ind$") &&
			!match(dep, "\\.lo.$") &&
			!match(dep, "\\.out$") &&
			!match(dep, "\\.toc$")) {
			# or whatever formatting is convenient
			# verify validity of file
			if (valid_file(dep) == 1) {
				printf("%s", " \\\n\t" dep);
			} else {
				# uh oh, filename was snipped!
				# printf("%s_NOT_FOUND", " \\\n\t" dep);
				hold = dep;
			}
		}
		# print "\n# LEFT: " $0;
	}
}

END {
	# end file with newline
	print "";
	print "";
}

