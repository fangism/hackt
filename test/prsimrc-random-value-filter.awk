#!/usr/bin/awk -f
#	$Id: prsimrc-random-value-filter.awk,v 1.2 2008/03/17 23:03:34 fang Exp $
# Script for filtering out random time time-stamps.  

{
	# >=4 to account for cause diagnosis
	if (NF >= 5 && $1 == "channel" && $3 == "(.data)" && $4 == ":") {
		# print "\t#TIME#\t" $2 " : " $4;
		# sub only replaces the FIRST occurrence, which is intentional
		# TODO: account for scientific notation
		sub(": [0-9]+", ": #VALUE#", $0);
		print;
	} else {
		print;
	}
}

