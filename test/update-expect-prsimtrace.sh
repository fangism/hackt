#!/bin/sh
# "update-expect-prsimtrace.sh"
# convenient maintainer script for updating expected outputs

args=`getopt "s:" $*`
if test $? != 0
then
	echo "Usage: $0 [ -s srcdir ] [files...]"
	exit 2
fi
set -- $args

# assumed source directory
srcdir=.
for i
do
	case "$i"
	in
		-s) srcdir="$2" shift; shift;;
		--) shift; break;;
	esac
done

# file extensions
outext=prsimtrace-dump-filter
expext=prsimtrace-expect

for i
do
	if test -f $i.$outext
	then
		cp -f $i.$outext $srcdir/$i.$expext
		echo "Updated $srcdir/$i.$expext"
	else
		echo "Missing $i.$outext."
	fi
done

