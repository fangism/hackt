#!/bin/sh
# "update-expect-vpi.sh"
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
	if test -f $i.vx-out-filter
	then
		cp -f $i.vx-out-filter $srcdir/$i.expect-vpi
		echo "Updated $srcdir/$i.expect-vpi"
#	elif test -f $i.vxfail-out-filter
#	then
#		cp -f $i.vxfail-out-filter $srcdir/$i.expect-vpi
#		echo "Updated $srcdir/$i.expect-vpi"
	else
		echo "Missing $i.vx[fail]-out-filter."
	fi
done

