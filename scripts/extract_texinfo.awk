#!/usr/bin/awk -f
#	$Id: extract_texinfo.awk,v 1.2 2007/08/15 02:48:37 fang Exp $
# "extract_texinfo.awk"
#	Extracts texinfo documentation excerpts from source and 
# writes them out to texinfo files.  
#
# Texinfo documentation can be slipped into block comments of the 
# source language.
# Documentation blocks are enclosed by:
# @texinfo filename
# @end texinfo
# where filename is the name of the file to print to.  

# Just for testing matching...
# /^@texinfo/,/^@end[ \t]+texinfo/ {
#	print;
# }

BEGIN {
	this_script = "$(top_srcdir)/scripts/extract_texinfo.awk";
}

/^@texinfo/ {
	outfile = $2;
	if (NF < 2) {
		print "Error: expected file name argument to @texinfo";
		exit 1;
	}
	print "Extracting " outfile " ...";
	print "@c \"" $2 "\"" > outfile;
	print "@c extracted from \"" FILENAME "\"" >> outfile;
	print "@c by \"" this_script "\"." >> outfile;
	print "" >> outfile;
	while (getline) {
		if (match($0, "^@end[ \t]+texinfo"))
			break;
		else	print >> outfile;
	}
	print "" >> outfile;
	close(outfile);
}


