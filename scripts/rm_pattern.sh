#!/bin/sh
#	$Id: rm_pattern.sh,v 1.2 2006/07/04 07:25:36 fang Exp $
# Making sure that rm -f <pattern> does the job.
# This is a workaround to problems where files are leftover after rm.  

for p in "$@"
do
	# echo "p: ""$p"
	# while { ls $p ;}
	while { ls $p > /dev/null 2>&1 ;}
	do
		# echo "rm -f $p"
		rm -f $p
		rm -f $p
	done
done

