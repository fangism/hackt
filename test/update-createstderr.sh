#!/bin/sh
# "update-createstderr.sh"
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
	if test -f $i.createdump-filter
	then
		cp -f $i.createdump-filter $srcdir/$i.createstderr
		echo "Updated $srcdir/$i.createstderr"
	elif test -f $i.createfaildump
	then
		cp -f $i.createfaildump $srcdir/$i.createstderr
		echo "Updated $srcdir/$i.createstderr"
	else
		echo "$i.createdump-filter or $i.createfaildump required but missing."
	fi
done

