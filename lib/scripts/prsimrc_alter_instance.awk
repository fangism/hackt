#!/usr/bin/awk -f
# "prsimrc_alter_instance.awk"
#	$Id: prsimrc_alter_instance.awk,v 1.1 2009/04/27 21:18:58 fang Exp $
# Produces a modified script given a template script (stdin) and instance name.
# template prsim script usually has set/assert commands.
# TODO: command-sensitive parsing and substitution!

# usage:
#       awk -f <this_script> -v inst_name="foo" < template.prsimrc
#       awk -f <this_script> -v prune=N < template.prsimrc
# options:
# -v inst_name=... the name of the instance to prepend to local names
# -v prune=N the number of levels of hierarchy to prune [0]

# e.g. -v inst_name=foo
#       set g.a 1
# becomes:
#       set foo.g.a 1

# e.g. -v prune=1
#       set foo.g.a 1
# becomes:
#       set g.a 1

# e.g. -v prune=1 -v inst_name=bar
#       set foo.g.a 1
# becomes:
#       set bar.g.a 1


BEGIN {
	# inst_name is now optional
	if (!length(prune)) {
		prune = 0;
	} else {
		prune = strtonum(prune);
	}
}

# change name depending on mode
function transform_name(str, 
	# local vars
	p) {
	# can prune before prepending!
	for (p=0; p<prune; ++p) {
		# strip one level at a time
		sub("^[^.]*\\.", "", str);
	}
	if (length(inst_name)) {
		return inst_name "." str;
	} else {
		return str;
	}
}

function transform_trailing_args(line, start,
	# local vars
	ntoks, toks, i) {
	ntoks = split(line, toks);
	line = $1;
	for (i=2; i<start && i<=ntoks; ++i) {
		line = line " " toks[i];
	}
	for (; i<=ntoks; ++i) {
		line = line " " transform_name(toks[i]);
	}
	return line;
}

{
	if ($1 == "get") {
		print $1 " " transform_name($2);
	} else if ($1 == "set" || $1 == "assert") {
		print $1 " " transform_name($2) " " $3;
	} else if ($1 == "watch" || $1 == "unwatch" ||
		$1 == "breakpt" || $1 == "unbreak") {
		print transform_trailing_args($0, 2);
	}
	else print;	# preserve
}

