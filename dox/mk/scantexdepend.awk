#!/usr/bin/awk -f
#	$Id: scantexdepend.awk,v 1.3 2006/11/27 22:59:57 fang Exp $
# "scantexdepend.awk"
# Scans [pdf][la]tex .log files for include dependencies.  
# usage: awk -f scantexdepend.awk [-v targets="..."] yourfile.log

BEGIN {
	# optional: space-delimited targets
	if (length(targets)) {
		printf("%s:", targets);
	}
}

/\([/\.]/ {
	# print "GOT: " $0;
	while (match($0,"\\(")) {
		# eat up everything up to and including the open-parenthesis
		sub("^[^(]*\\(", "", $0);
		# print "HAVE: " $0;
		ntoks = split($0, toks);
		dep = toks[1];
		# eat closing paren if attached to first token
		gsub("[)]+", "", dep);
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
			printf("%s", " \\\n\t" dep);
		}
	}
}

END {
	# end file with newline
	print "";
	print "";
}

