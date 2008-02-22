#!/usr/bin/awk -f
#	$Id: prsimrc-random-value-filter.awk,v 1.1.4.1 2008/02/22 06:07:52 fang Exp $
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

