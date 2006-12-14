#!/bin/sh
# "update-expect-chpsimalloc.sh"
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
	cp -f $i.chpsimallocdump $srcdir/$i.expect-chpsimalloc
	echo "Updated $srcdir/$i.expect-chpsimalloc"
done

