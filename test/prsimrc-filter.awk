#!/usr/bin/awk -f
# "prsimrc-filter.awk"
#	$Id: prsimrc-filter.awk,v 1.2 2006/04/03 05:30:41 fang Exp $
# filter output produced by prsimrc dumps
# NOTE: this could be a simple sed script for now...

/^Error/ {
# Intended to substitute:
# Error opening file: "..."
# with:
# Error opening file: #FILE#
	gsub("\".*\"", "#FILE#", $0);
	print;
}

!/^Error/ {
	print;
}

