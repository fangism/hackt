#!/bin/sh
# "update-flatten-expect.sh"
#	$Id: update-flatten-expect.sh,v 1.1 2006/11/15 00:09:23 fang Exp $
# convenient maintainer script for updating expected outputs
# use this script for updating yacc test outputs, not bison

# TODO: make update conditional on yacc/bison configuration?
# too much hassle because not all parse failure outputs are 
# dependent on the parser-generator

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
	if test -f $i.hacf-filter
	then
		cp -f $i.hacf-filter $srcdir/$i.hacf-expect
		echo "Updated $srcdir/$i.hacf-expect"
	elif test -f $i.hacf-fail-filter
	then
		cp -f $i.hacf-fail-filter $srcdir/$i.hacf-expect
		echo "Updated $srcdir/$i.hacf-expect"
	else
		echo "Missing $i.hacf-filter or $i.hacf-fail-filter."
	fi
done

