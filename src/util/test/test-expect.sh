#!/bin/sh
# "test-expect.sh"

# $1 is the executable, expecting no input
# $2 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build

cmd=$1
filter=$2/../../../test/address_filter.sed
srcroot=$2/$1
bldroot=$1

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

diff -u $bldroot.stderr.filter $bldroot.out.filter 2>&1 | cat > $bldroot.diff

if [ -s $bldroot.diff ] ; then
	echo "$bldroot.diff is non-empty!"
	exit 1
fi

