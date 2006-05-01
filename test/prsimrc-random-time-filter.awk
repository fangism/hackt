#!/usr/bin/awk -f
#	$Id: prsimrc-random-time-filter.awk,v 1.1.12.1 2006/05/01 03:00:02 fang Exp $
# Script for filtering out random time time-stamps.  

{
	# >=4 to account for cause diagnosis
	if (NF >= 4 && $3 == ":") {
		# print "\t#TIME#\t" $2 " : " $4;
		# sub only replaces the FIRST occurrence, which is intentional
		# TODO: account for scientific notation
		sub("[0-9]+[.]?[0-9]*", "#TIME#", $0);
		print;
	} else {
		print;
	}
}

