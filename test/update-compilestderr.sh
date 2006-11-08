#!/bin/sh
# "update-compilestderr.sh"
#	$Id: update-compilestderr.sh,v 1.3.30.1 2006/11/08 19:45:20 fang Exp $
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
#	cp -f $i.test.filter $srcdir/$i.stderr
	cp -f $i.compiledump-filter $srcdir/$i.stderr
	echo "Updated $srcdir/$i.stderr"
done

