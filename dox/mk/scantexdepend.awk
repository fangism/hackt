#!/usr/bin/awk -f
#	$Id: scantexdepend.awk,v 1.2 2006/11/26 04:41:10 fang Exp $
# "scantexdepend.awk"
# Scans [pdf][la]tex .log files for include dependencies.  

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
		# suppress auxiliary dependencies, because they are cleaned
		# and we do not write suffix rules for them (yet)
		if (!match(dep, "\\.aux$") &&
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

