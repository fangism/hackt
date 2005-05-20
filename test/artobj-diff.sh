#!/bin/sh
# "artobj-diff.sh"
# compares textual object dumps
#	$Id: artobj-diff.sh,v 1.6 2005/05/20 19:29:15 fang Exp $

# $1 is the executable for generating the object file, (probably art++2obj)
#	and producing a textual dump to stderr.  
# $2 is the executable for reading in the object file, (probably artobjdump)
#	and producing a textual dump to stderr.
# $3 is the path to the source directory, which is not necessarily ./
#	especially during a vpath build for distcheck.
# $4 is the input file name WITHOUT the .in extension
#	prerequisite: the input file must pass type-checking
# $5 is the filter command for ignoring difference in output
#	(for no filter, pass "cat")
# $6 is the test report log file, summarizing results
# additional arguments are ignored.  

# if the .test file exists, then program didn't crash, we can proceed.

cmd="$1 -d"
# -d for dump flag
dump=$2
srcroot=$3/$4
bldroot=$4
filter=$5
logfile=$6

if ! [ -e $bldroot.test ] ; then exit 1 ; fi

$cmd $srcroot.in 2> /dev/null
if [ $? -ne 0 ] ; then exit 1; fi
$cmd $srcroot.in $bldroot.artobj 2>&1 | cat > $bldroot.outdump

$dump $bldroot.artobj 2> /dev/null
if [ $? -ne 0 ] ; then exit 1; fi
$dump $bldroot.artobj 2>&1 | cat > $bldroot.indump

cat $bldroot.outdump | $filter > $bldroot.outdump.filter
cat $bldroot.indump | $filter > $bldroot.indump.filter
diff -b -u $bldroot.outdump.filter $bldroot.indump.filter 2>&1 | cat > $bldroot.objdiff

if [ -s $bldroot.objdiff ] ; then
	echo "$bldroot.objdiff is non-empty!  See $logfile."
	echo `pwd`/"$bldroot.objdiff" >> $logfile
	exit 1
fi

