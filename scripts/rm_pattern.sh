#!/bin/sh
#	$Id: rm_pattern.sh,v 1.3 2006/07/27 05:55:33 fang Exp $
# Making sure that rm -f <pattern> does the job.
# This is a workaround to problems where files are leftover after rm.  

for p in "$@"
do
	while { rm -- $p ; } do : ; done > /dev/null 2>&1
done

