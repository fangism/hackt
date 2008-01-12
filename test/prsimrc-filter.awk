#!/usr/bin/awk -f
# "prsimrc-filter.awk"
#	$Id: prsimrc-filter.awk,v 1.2.98.1 2008/01/12 22:59:59 fang Exp $
# filter output produced by prsimrc dumps
# NOTE: this could be a simple sed script for now...

{
if (match($0, "^Error") || match($0, "^## enter:") || match($0, "^## leave:")) {
# Intended to substitute:
# Error opening file: "..."
# with:
# Error opening file: #FILE#
# likewise:
# ## enter: "..."
# ## leave: "..."
	gsub("\".*\"", "#FILE#", $0);
	print;
} else {
	print;
}
}

