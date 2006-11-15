#!/bin/sh
# "update-compilestderr-bison.sh"
#	$Id: update-compilestderr-bison.sh,v 1.4 2006/11/15 21:57:00 fang Exp $
# convenient maintainer script for updating expected outputs
# use this script when updating bison tests

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
	cp -f $i.compiledump-filter $srcdir/$i.stderr.bison
	echo "Updated $srcdir/$i.stderr.bison"
done

