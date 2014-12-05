#!/usr/bin/awk -f
# "spice-wrap.awk"
#	$Id: spice-wrap.awk,v 1.3 2011/02/25 23:19:26 fang Exp $
# automatically wrap long line of spice files
# comments get wrapped too, assuming '*' for comments.  
# usage:
#	spice-wrap.awk [options] < in.spice > out.spice
# options:
#	-v wrap=INT	line length to wrap to (optional, default=80)
#	-v continuation=STRING	wrapping string
#		uses "\n+ " line continuation by default
#	-v comment=STRING	wrapping string
#		uses "\n* " comment continuation by default

BEGIN {
	if (length(wrap)) {
		wrap = strtonum(wrap);
	} else {
		wrap = 80;
	}
	if (!length(continuation)) {
		continuation = "\n+ ";
	}
	if (!length(comment)) {
		comment = "\n* ";
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
	if (substr($0,1,1) == "*") {
		# comments wrap differently
		cont = comment;
	} else {
		cont = continuation;
	}
	len = 0;
	ntoks = split($0, toks);
	for (i=1; i<=ntoks; ++i) {
		t = toks[i];
		tl = length(t);
		if (len + tl +lhead >= wrap) {
			printf("%s", cont);
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

