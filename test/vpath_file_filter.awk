#!/usr/bin/awk -f
#	$Id: vpath_file_filter.awk,v 1.7 2010/04/30 00:21:47 fang Exp $
# Filters out path differences in file names, preserving only the
# non-directory portion of the file name. 
# Useful in making path-dependent tests distcheck-able.  

# NOTE: this is easier to do in sed, but need to be able to chain
# multiple awk files in one command

# NOTE: removing all '/'s is incorrect when referenced files reside
# in subdirectories of the srcdir.  Best to restrict use of this
# to test cases that don't reference files outside of the local directories.  

# TODO: configure this to be a $(srcdir)-only filter

function extract_quoted_string(str, i) {
	match(str, "\"[^\"]*\"");
	return substr(str, RSTART, RLENGTH);
}

function strip_quotes(str) {
	return substr(str, 2, length(str) -2);
}

function basename(str) {
	gsub(".*\\/", "", str);
	return str;
}

function basename_quoted_filenames(str) {
	gsub("\"[^\"]*\"", \
		"\"" basename(strip_quotes(extract_quoted_string(str))) "\"", \
		str);
	return str;
}

function filter_vpath_files(str) {
	if (match(str,"^At:") || match(str,"^From:")) {
#		|| match(str, "^Error in:")
		str = basename_quoted_filenames(str);
#		while(match(str,"\\/")) {
#			gsub("\"[^/]*\\/","\"",str);
#		}
	}
	return str;
}

# used by "hackt-flatten-expect-filter.awk"
# now catch file-embedding directives
function flatten_filter_vpath_files(str) {
	if (match(str,"^At:") || match(str,"^From:") ||
		match(str,"^#FILE \"[^\"]*\"") ||
		match(str,"%}\t// #FILE \"[^\"]*\"")) {
#		match(str,"// enter:") || match(str,"// leave:")
		# expect file name in quotes
		str = basename_quoted_filenames(str);
	}
	return str;
}

