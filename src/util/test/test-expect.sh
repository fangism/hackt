#!/bin/sh
# "test-expect.sh"

# $1 is the executable, expecting no input
# $2 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build

# see if it crashes first
{ $1 ; if [ $? -gt 1 ] ; then exit 1 ; fi } 2>&1 | cat > /dev/null

# the run real test, comparing outputs
$1 2>&1 | cat > $1.out

diff $2/$1.stderr $1.out 2>&1 | cat > $1.diff

if [ -s $1.diff ] ; then
	echo "$1.diff is non-empty!"
	exit 1
fi

