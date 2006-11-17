#!/bin/sh
# "update-unrollstderr.sh"
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
	if test -f $i.unrolldump-filter
	then
		cp -f $i.unrolldump-filter $srcdir/$i.unrollstderr
		echo "Updated $srcdir/$i.unrollstderr"
	elif test -f $i.unrollfaildump
	then
		cp -f $i.unrollfaildump $srcdir/$i.unrollstderr
		echo "Updated $srcdir/$i.unrollstderr"
	else
		echo "$i.unrolldump-filter or $i.unrollfaildump required but missing."
	fi
done

