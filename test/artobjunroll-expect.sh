#!/bin/sh
# "artobjunroll-expect.sh"
#	$Id: artobjunroll-expect.sh,v 1.2 2005/01/28 19:59:00 fang Exp $

# $1 is the executable for the unroller, expecting 2 arguments
# $2 is the executable for reading in the object file, (probably artobjdump)
#	and producing a textual dump to stderr.

# $3 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build
# $4 is the input file name WITHOUT the .in extension
# $5 is the filter for ignoring certain difference in output
#	(for no filter, pass "cat")
# $6 is the test report log file, summarizing results
# other subsequent arguments are ignored

# if [ $# -lt 6 ] ; then exit 1 ; fi

unroll=$1
dump=$2
srcroot=$3/$4
bldroot=$4
filter=$5
logfile=$6

# an pre-unrolled object file must already exist
if ! [ -e $bldroot.artobj ] ; then exit 1 ; fi

# see if it crashes first
$unroll $bldroot.artobj $bldroot.artobjunroll 2> $bldroot.unrolldump

# depending on exit status, take action
case $? in
	0) 
		# the run real test, comparing outputs
		# $unroll $bldroot.artobj $bldroot.artobjunroll
		$dump $bldroot.artobjunroll 2>&1 | cat > $bldroot.unrolldump
		break ;;
	1) break ;;
	*) exit 1 ;;
esac

$filter $bldroot.unrolldump > $bldroot.unrolldump.filter

# .unrollstderr comparison file must exist
if [ -f $srcroot.unrollstderr ] ; then
	$filter $srcroot.unrollstderr > $bldroot.unrollstderr.filter
else
	echo "Missing $bldroot.unrollstderr."
	exit 1
fi

# ignore whitespace differences
diff -buw $bldroot.unrollstderr.filter $bldroot.unrolldump.filter 2>&1 | cat > $bldroot.unrolldiff

if [ -s $bldroot.unrolldiff ] ; then
	echo "$bldroot.unrolldiff is non-empty!  See $logfile."
	echo `pwd`/"$bldroot.unrolldiff" >> $logfile
	exit 1
fi

