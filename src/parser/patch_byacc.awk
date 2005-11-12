#!/usr/bin/awk -f
#	$Id: patch_byacc.awk,v 1.1 2005/11/12 22:30:27 fang Exp $
# "parser/patch_byacc.awk"
# a script to patch byacc-generated parsers

BEGIN {
	this_script = "\"parser/patch_byacc.awk\"";
	yydebug_count = 0;
}

# read all lines
{
if (match($0, "extern.*getenv")) {
	# bad local extern declaration, wrong prototype
	print comment_out($0);
} else if (match($0, "[#]if YYDEBUG")) {
	# we want to keep the yyname, and yyrule strings,
	# which are unfortunately wrapped in YYDEBUG, 
	# but we don't want the rest of the debug code.
	if (yydebug_count) {
		# do nothing
		yydebug_count += 0;
		print;
	} else {
		# first time, we then disable all subsequent YYDEBUGs
		# while allowing this body to proceed
		print;
		print "/* undef-redef injected by " this_script " */";
		print "#undef YYDEBUG";
		print "#define YYDEBUG\t0";
		# oh this is soooo evil...
	}
	yydebug_count++;
} else if (match($0, "yydefred\\[yystate\\]")) {
	# need extra parens in assignment used as truth value
	sub("\\(.*\\)", "(&)", $0);
	print $0;
} else {
	# everything else:
	print;
}
}

function comment_out(str) {
	return "/* " str " */ // removed by " this_script;
}

