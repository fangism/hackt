#!/usr/bin/awk -f
# "balance_indent.awk"

# for all those unreadable template error messages in the C++ standard library

BEGIN {
	level = 0;
	indent = "    ";
	# or "\t"
	width = 80;	# terminal width, overrideable
}

# note beware of operators < and >

function auto_indent(lvl, i) {
	ret = "";
	i=0;		# local var
	for ( ; i<lvl; i++)
		ret = indent ret;
	return ret;
}

function balance_indent(str,
	# local variables
		arr, numlines, i, ilevel, joinlines) {
	gsub("\\(", "(\n", str);
	gsub("\\[", "[\n", str);
	gsub("<", "<\n", str);
	gsub(",[ \\t]*", ",\n", str);
	gsub("\\)", "\n)", str);
	gsub("\\]", "\n]", str);
	gsub(">", "\n>", str);

	# split them apart
	numlines = split(str, arr, "\\n");
	i=1;
	for ( ; i<=numlines; i++) {
		if (match(arr[i], "^\\)") || match(arr[i], "^>") ||
				match(arr[i], "^\\]")) {
			level--;
		}
		# prepend indent
		ilevel[i] = level;
		if (match(arr[i], "\\($") || match(arr[i], "<$") ||
				match(arr[i], "\\[$")) {
			level++;
		}
		# else no change in level
	}

	# glue back together
	# any intelligence about short balances?

#	joinlines = smart_join(arr, ilevel, 0, numlines-1);

	str = arr[1];
	i = 2;
	for ( ; i<=numlines; i++) {
		if (length(arr[i]))
			str = str "\n" auto_indent(ilevel[i]) arr[i];
		# else skip blank lines
	}
	return str;
}

# intelligently joins lines, recursively
function smart_join(arr, ilevel, startindex, endindex, 
	# returned arrays (by reference)
	subarr, subilevel, subnumlines, 
	# local variables
	i, is_nested, curlevel, linelength) {

	# so we know this is passed by reference
	subnumlines[0] = 0;
	is_nested = 0;
	curlevel = ilevel[startindex];

	# phase 0: scan and see if this sub-array has nested levels
	for (i=startindex+1; i<=endindex && !is_nested; i++) {
		if (ilevel[i] > curlevel)
			is_nested = 1;
	}

	if (is_nested) {

		# phase 1: partition into sections by level
		# what is one *item* in this level?
		for (i = startindex; i<=endindex; i++) {
			
		}
	} else {
		# we're in deepest level, no need to recurse
		# we have uniformly indented subarray of lines
		# see what we can combine...
		# no partial joining, just all or none

		# compute total line length (until width exceeded)
		linelength = length(auto_indent(curlevel));
		for (i=startindex; i<=endindex && linelength < width; i++) {
			linelength += length(arr[i]);
		}
		if (linelength < width) {
			# join them all
			# don't prefix with indent yet.
			subilevel[startindex] = ilevel[startindex];
			for (i=startindex; i<=endindex; i++) {
				subarray[startindex] = \
					subarray[startindex] arr[i];
			}
			subnumlines[0] = 1;
		} else {
			# don't join any, just copy back
			for (i=startindex; i<=endindex; i++) {
				subarray[i] = arr[i];
				subilevel[i] = ilevel[i];
			}
			subnumlines[0] = startindex -endindex +1;
		}
	}
}

# for almost all lines:
{
	# exceptions
	if (match($0, "^g?make") || match($0, "^distcc"))
		print;
	else	print balance_indent($0);
}

