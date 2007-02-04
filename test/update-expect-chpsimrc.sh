#!/bin/sh
# "update-expect-chpsimrc.sh"
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
	if test -f $i.chpsimrc-out-filter
	then
		cp -f $i.chpsimrc-out-filter $srcdir/$i.chpsimrc-expect
		echo "Updated $srcdir/$i.chpsimrc-expect"
	elif test -f $i.chpsimrcfail-out
	then
		cp -f $i.chpsimrcfail-out $srcdir/$i.chpsimrc-expect
		echo "Updated $srcdir/$i.chpsimrc-expect"
	else
		echo "Missing $i.chpsimrc[fail]-out"
	fi
done

