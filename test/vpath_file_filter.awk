#!/usr/bin/awk -f
#	$Id: vpath_file_filter.awk,v 1.1.2.1 2005/11/10 00:47:48 fang Exp $
# Filters out path differences in file names, preserving only the
# non-directory portion of the file name. 
# Useful in making path-dependent tests distcheck-able.  

# NOTE: this is easier to do in sed, but need to be able to chain
# multiple awk files in one command

function filter_vpath_files(str) {
	if (match(str,"^At:") || match(str,"^From:")) {
		while(match(str,"\\/")) {
			gsub("\"[^/]*\\/","\"",str);
		}
	}
	return str;
}

