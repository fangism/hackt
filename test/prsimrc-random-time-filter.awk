#!/usr/bin/awk -f
#	$Id: prsimrc-random-time-filter.awk,v 1.1 2006/04/03 22:11:20 fang Exp $
# Script for filtering out random time time-stamps.  

{
	if (NF == 4 && $3 == ":") {
		print "\t#TIME#\t" $2 " : " $4;
	} else {
		print;
	}
}

