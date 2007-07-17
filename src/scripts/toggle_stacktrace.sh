#!/bin/sh
# "toggle_stacktrace.h"
# script to quickly toggle ENABLE_STACKTRACE in source files for debugging
# usage: <script> [0|1] files...

# args=`getopt "" $*`
# if test $? != 0
# fi

if test $# -lt 2
then
	echo "Usage: $0 [0|1] [files ...]"
	exit 2
fi
# set -- $args

# first argument is new switch value: 0 or 1
val="$1"
echo "Setting ENABLE_STACKTRACE to $val"

# if (test "$val" -ne 0 && test "$val" -ne 1)
# then
#	echo "Usage: $0 [0|1] [files ...]"
#	exit 2
# fi

shift

for i
do
	if (test -f $i && test -w $i)
	then
		sed '/#define[	 ]ENABLE_STACKTRACE/s/[0-9]/'$val'/' $i > $i.tmp
		mv $i.tmp $i
		echo "sed-ed $i"
	else
		echo "$i does not exist or is not writeable."
	fi
done

