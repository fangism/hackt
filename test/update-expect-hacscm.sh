#!/bin/sh
# "update-expect-hacscm.sh"
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
	if test -f $i.hacscm-out
	then
		cp -f $i.hacscm-out $srcdir/$i.hacscm-expect
		echo "Updated $srcdir/$i.hacscm-expect"
	elif test -f $i.hacscmfail-out
	then
		cp -f $i.hacscmfail-out $srcdir/$i.hacscm-expect
		echo "Updated $srcdir/$i.hacscm-expect"
	else
		echo "Missing $i.hacscm[fail]-out"
	fi
done

