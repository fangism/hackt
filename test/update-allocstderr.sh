#!/bin/sh
# "update-allocstderr.sh"
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
	if test -f $i.allocdump-filter
	then
		cp -f $i.allocdump-filter $srcdir/$i.allocstderr
		echo "Updated $srcdir/$i.allocstderr"
	elif test -f $i.allocfaildump
	then
		cp -f $i.allocfaildump $srcdir/$i.allocstderr
		echo "Updated $srcdir/$i.allocstderr"
	else
		echo "$i.allocdump-filter or $i.allocfaildump required but missing."
	fi
done


