#!/usr/bin/awk -f
# "balance_indent.awk"

BEGIN {
	level = 0;
	indent = "    ";
	# or "\t"
	width = 80;	# terminal width
}

# note beware of operators < and >

function auto_indent(lvl, i) {
	ret = "";
	i=0;		# local var
	for ( ; i<lvl; i++)
		ret = indent ret;
	return ret;
}

function balance_indent(str, arr, nl, i) {
	gsub("\\(", "(\n", str);
	gsub("\\[", "[\n", str);
	gsub("<", "<\n", str);
	gsub(",[ \\t]*", ",\n", str);
	gsub("\\)", "\n)", str);
	gsub("\\]", "\n]", str);
	gsub(">", "\n>", str);

	# split them apart
	delete arr;
	nl = split(str, arr, "\\n");
	i=1;
	for ( ; i<=nl; i++) {
		if (match(arr[i], "^\\)") || match(arr[i], "^>") ||
				match(arr[i], "^\\]")) {
			level--;
		}
		# prepend indent
		arr[i] = auto_indent(level) arr[i];
		if (match(arr[i], "\\($") || match(arr[i], "<$") ||
				match(arr[i], "\\[$")) {
			level++;
		}
		# else no change in level
	}

	# glue back together
	# any intelligence about short balances?
	str = arr[1];
	i = 2;
	for ( ; i<=nl; i++)
		str = str "\n" arr[i];
	return str;
}

# for almost all lines:
{
	# exceptions
	if (match($0, "^g?make") || match($0, "^distcc"))
		print;
	else	print balance_indent($0);
}

