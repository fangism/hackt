#!/usr/bin/awk -f
# "automake_include.awk"
#	$Id: automake_include.awk,v 1.3.2.1 2005/12/11 22:29:28 fang Exp $

# input (stdin): newline-separated list of files to generate 
#	automake-style include dependencies
# output (stdout): a Makefile.in style output to include in Makefile.am

BEGIN {
	# added a '-' before @am__include@ allows make to
	# ignores files that are missing, (but not recommended)
	include_prefix = "@AMDEP_TRUE@@am__include@ @am__quote@";
	include_suffix = "@am__quote@";
}

{
	n = split($0, toks);
	for (i=1; i<=n; i++) {
	# ignore junk
	if (match(toks[i], "chkd")) {
		print include_prefix transform_to_include_dep(toks[i]) \
			include_suffix;
	}
	}
}

# takes a file name X/Y where X is a path, Y is file names
# and produces X/$(DEPDIR)/Yd, so bar/foo.h will become something like
# bar/.deps/foo.hd
function transform_to_include_dep(f) {
	# sed 's|[^/]*$|.deps/&|'
	sub("[^/]*$", "$(DEPDIR)/&", f);
	return f;
}

