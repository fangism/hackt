#!/bin/sh
# "test-expect.sh"
#	$Id: test-expect.sh,v 1.2.4.1.2.2 2005/01/27 06:04:54 fang Exp $

# $1 is the executable, expecting no input
# $2 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build

cmd=$1
filter=$2/../../../test/address_filter.sed
srcroot=$2/$1
bldroot=$1
morefilter=$srcroot.morefilter.sh

# see if it crashes first
{ $cmd ; if [ $? -gt 1 ] ; then exit 1 ; fi } 2>&1 | cat > /dev/null

# the run real test, comparing outputs
$cmd 2>&1 | cat > $bldroot.out

$filter $bldroot.out > $bldroot.out.filter

if ! [ -f $srcroot.stderr ]
then
	echo "Missing $srcroot.stderr"
	exit 1
fi

$filter $srcroot.stderr > $bldroot.stderr.filter

# optional filter hook script, to do more than the default filtering
# requirement of morefilter script: should mv .filter files to .prefilter
#	and create replacement .filter files from the .prefilter files
if [ -f $morefilter ]
then
	mv $bldroot.stderr.filter $bldroot.stderr.prefilter
	mv $bldroot.out.filter $bldroot.out.prefilter
	$morefilter $bldroot.stderr.prefilter $bldroot.stderr.filter
	$morefilter $bldroot.out.prefilter $bldroot.out.filter
	diff -u $bldroot.stderr.prefilter $bldroot.out.prefilter 2>&1 | cat > $bldroot.prefilter.diff
	# what the heck, print a warning message, just FYI
	if [ -s $bldroot.prefilter.diff ]
	then
		echo "$bldroot.prefilter.diff is actually non-empty.  (OK)"
	fi
fi

diff -u $bldroot.stderr.filter $bldroot.out.filter 2>&1 | cat > $bldroot.diff

if [ -s $bldroot.diff ]
then
	echo "$bldroot.diff is non-empty!"
	exit 1
fi

