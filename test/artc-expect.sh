#!/bin/sh
# "artc-expect.sh"
#	$Id: artc-expect.sh,v 1.4.4.1.6.1 2005/01/27 00:56:44 fang Exp $

# $1 is the executable, expecting input from stdin
# $2 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build
# $3 is the input file name WITHOUT the .in extension
# $4 is the filter for ignoring certain difference in output
#	(for no filter, pass "cat")
# $5 is the test report log file, summarizing results
# other subsequent arguments are ignored

# if [ $# -lt 6 ] ; then exit 1 ; fi

cmd=$1
srcroot=$2/$3
bldroot=$3
filter="$4"
logfile=$5

# echo $cmd
# echo $srcroot
# echo $bldroot
# echo $filter
# echo $logfile

# see if it crashes first
$cmd < $srcroot.in 2> /dev/null ; \
if [ $? -gt 1 ] ; then exit 1 ; fi > /dev/null

# the run real test, comparing outputs
$cmd < $srcroot.in 2>&1 | cat > $bldroot.test
$filter $bldroot.test > $bldroot.test.filter
if [ -f $srcroot.stderr ] ; then
	$filter $srcroot.stderr > $bldroot.stderr.filter
else
	touch $bldroot.stderr.filter
fi

diff -u $bldroot.stderr.filter $bldroot.test.filter 2>&1 | cat > $bldroot.diff

# first-try: if different, see if it's because of bison...
# if so, re-run using bison's expected output
# otherwise, exit with error
if [ -s $bldroot.diff ] ; then
	if grep ^YACC Makefile | grep -q bison ; then
		if [ -f $srcroot.stderr.bison ] ; then
			$filter $srcroot.stderr.bison > $bldroot.stderr.bison.filter
		else
			touch $bldroot.stderr.bison.filter
		fi
		diff $bldroot.stderr.bison.filter $bldroot.test.filter 2>&1 | cat > $bldroot.bison.diff
		if ! [ -s $bldroot.bison.diff ] ; then
			mv -f $bldroot.bison.diff $bldroot.diff
		else
			echo "$bldroot.bison.diff is non-empty!"
		fi
	fi
fi

if [ -s $bldroot.diff ] ; then
	echo "$bldroot.diff is non-empty!  See $logfile."
	echo `pwd`/"$bldroot.diff" >> $logfile
	exit 1
fi

