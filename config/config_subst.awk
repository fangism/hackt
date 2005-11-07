#!/usr/bin/awk -f
#	$Id: config_subst.awk,v 1.1.2.1 2005/11/07 08:54:28 fang Exp $
# script to perform config.status substitutions before it is done running
# useful for using config.status to produce configure-time output files.  

# arguments:
# -v var -- the name of the configure-variable found in config.status
#	without the surrounding @...@

BEGIN {
	if (!length(var)) {
		print "Missing argument: -v <subst-var> .";
		exit 1;
	}
	match_var = "@" var "@";
	found = 0;
}

/^s,@[A-Za-z0-9_]+@/ {
	if (match($0,match_var)) {
		# strip away non-essential syntactic sugar
		gsub("^.*@,","");
		gsub(",;.*$","");
		print;
		found++;
	}
}

END {
	if (!found && length(var)) {
		print "Error: variable not found: " var;
		exit 1;
	} else if (found > 1) {
		print "Error: found and printed more than one match!";
		exit 1;
	}
}

