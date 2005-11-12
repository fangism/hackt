#!/bin/sh
#	$Id: exhaust_check.sh,v 1.1 2005/11/12 22:30:24 fang Exp $
# "scripts/exhaust_check.sh"
# exhaustively checks a ton of configurations on one platform
# feel free to edit as necessary
# this script was written for a particular Mac OS X system
# NOTE: ccache is required for this script to run
# it turns this exponential task into linear time.  

bootstrap
for c in g++ g++-4
do
	# maybe even include byacc
	for y in /usr/bin/yacc bison
	do
		for l in flex /sw/lib/flex/bin/flex
		do
			rm -f config.cache
			configure CXX="ccache $c" YACC=$y LEX=$l -C
			make && make check.log && make distcheck.log
		done
	done
done

