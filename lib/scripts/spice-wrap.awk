#!/usr/bin/awk -f
# "spice-wrap.awk"
#	$Id: spice-wrap.awk,v 1.2 2010/09/21 00:18:01 fang Exp $
# automatically wrap long line of spice files (or any file!)
# usage:
#	spice-wrap.awk [options] < in.spice > out.spice
# options:
#	-v wrap=INT	line length to wrap to (optional, default=80)
#	-v continuation=STRING	wrapping string
#		uses "\n+ " line continuation by default

BEGIN {
	if (length(wrap)) {
		wrap = strtonum(wrap);
	} else {
		wrap = 80;
	}
	if (!length(continuation)) {
		continuation = "\n+ ";
	}
	# to be precise about line length
	conttail = continuation;
	conthead = continuation;
	sub(".*\\n", "", conttail);
	sub("\\n.*", "", conthead);
	ltail = length(conttail);
	lhead = length(conthead);
}

# for all lines
{
if (length($0) > wrap && wrap > 0) {
	len = 0;
	ntoks = split($0, toks);
	for (i=1; i<=ntoks; ++i) {
		t = toks[i];
		tl = length(t);
		if (len + tl +lhead >= wrap) {
			printf("%s", continuation);
			len = tl +ltail;
		} else {
			len += tl;
		}
		printf("%s", t);
		if (i != ntoks) { printf(" "); ++len; }
	}
	print "";
} else {
	print;
}
}

