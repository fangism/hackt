#!/bin/sh
# "update-expect-vcd.sh"
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
	cp -f $i.vcd-filter $srcdir/$i.vcd-expect
	echo "Updated $srcdir/$i.vcd-expect"
done

