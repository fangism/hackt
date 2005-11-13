#!/bin/sh
#	$Id: exhaust_check.sh,v 1.2 2005/11/13 20:21:10 fang Exp $
# "scripts/exhaust_check.sh"
# exhaustively checks a ton of configurations on one platform
# feel free to edit as necessary
# this script was written for a particular Mac OS X system
# NOTE: ccache is required for this script to run
# it turns this exponential task into linear time.  

bootstrap
configure -C
make distclean
for c in g++ g++-4
do
	# maybe even include byacc
	for y in /usr/bin/yacc bison
	do
		for l in flex /sw/lib/flex/bin/flex
		do
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
			echo "Testing: CXX=\"ccache $c\" YACC=$y  LEX=$l"
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	rm -rf build
	mkdir -p build
	if ( cd build && ../configure CXX="ccache $c" YACC=$y LEX=$l -C && \
		make && make check.log )
	then
		echo "Passed: CXX=\"ccache $c\" YACC=$y  LEX=$l"
	else
		echo "Failed: CXX=\"ccache $c\" YACC=$y  LEX=$l"
	fi
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
		done
	done
done

# finally end with default configuration
rm -rf build
configure -C
make && make check.log && make distcheck.log

