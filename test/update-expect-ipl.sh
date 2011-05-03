#!/bin/sh
# "update-expect-iplrc.sh"
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

for i
do
	if test -f $i.iplrc-out-filter
	then
		cp -f $i.iplrc-out-filter $srcdir/$i.iplrc-expect
		echo "Updated $srcdir/$i.iplrc-expect"
	elif test -f $i.iplrcfail-out-filter
	then
		cp -f $i.iplrcfail-out-filter $srcdir/$i.iplrc-expect
		echo "Updated $srcdir/$i.iplrc-expect"
	else
		echo "Missing $i.iplrc[fail]-out-filter."
	fi
done

