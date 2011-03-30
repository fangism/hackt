#!/bin/sh
# "update-expect-verilog.sh"
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

# TODO: use list of extensions
ext=hv
ext2=hv-fail
exp=hv-expect

for i
do
	if test -f $i.$ext
	then
		cp -f $i.$ext $srcdir/$i.$exp
		echo "Updated $srcdir/$i.$exp"
	elif test -f $i.$ext2
	then
		cp -f $i.$ext2 $srcdir/$i.$exp
		echo "Updated $srcdir/$i.$exp"
	else
		echo "$i.$ext or $i.$ext2 required but missing."
	fi
done

