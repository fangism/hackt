#!/bin/sh
# "update-expect-lvsprs.sh"
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
	if test -f $i.lvsprs
	then
		cp -f $i.lvsprs $srcdir/$i.expect-lvsprs
		echo "Updated $srcdir/$i.expect-lvsprs"
	elif test -f $i.lvsfail
	then
		cp -f $i.lvsfail $srcdir/$i.expect-lvsprs
		echo "Updated $srcdir/$i.expect-lvsprs"
	else
		echo "$i.lvsprs or $i.lvsfail required but missing."
	fi
done

