#!/usr/bin/awk -f
# "deps_to_dot.awk"
# converts Makefile dependencies to graphviz dot format
# recommend pre-unique-sorting the input to avoid duplicate edges

# the resulting file can be fed to 'dot -Tpdf foo.dot -o foo.pdf

BEGIN {
print "digraph G {";
print "rankdir=LR;";
}

{
	tmp = $0;
	gsub(":", "", tmp);
	ntoks = split(tmp, toks);
	tgt = toks[1];
	for (i=2; i<= ntoks; ++i) {
		print "\"" tgt "\" -> \"" toks[i] "\";";
	}
}

END {
print "}";
}

