#!/bin/sh
# "artobj-diff.sh"
# compares textual object dumps

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

if ! [ -e $3/$4.test ] ; then exit 1 ; fi

$1 $3/$4.in 2> /dev/null
if [ $? -ne 0 ] ; then exit 1; fi
$1 $3/$4.in $4.artobj 2>&1 | cat > $4.outdump

$2 $4.artobj 2> /dev/null
if [ $? -ne 0 ] ; then exit 1; fi
$2 $4.artobj 2>&1 | cat > $4.indump

$5 $4.outdump > $4.outdump.filter
$5 $4.indump > $4.indump.filter
diff $4.outdump.filter $4.indump.filter 2>&1 | cat > $4.objdiff

if [ -s $4.objdiff ] ; then
	echo "$4.objdiff is non-empty!  See $6."
	echo `pwd`/"$4.objdiff" >> $6
	exit 1
fi

