#!/bin/sh
# "artc-expect.sh"

# $1 is the executable, expecting input from stdin
# $2 is the path to the source directory, which is not necessarily ./
#	during distcheck uses _build
# $3 is the input file name WITHOUT the .in extension
# $4 is the filter for ignoring certain difference in output
#	(for no filter, pass "cat")
# $5 is the test report log file, summarizing results
# other subsequent arguments are ignored

# if [ $# -lt 6 ] ; then exit 1 ; fi

$1 < $2/$3.in 2>&1 | cat > $3.test
$4 $3.test > $3.test.filter
if [ -f $2/$3.stderr ] ; then \
	$4 $2/$3.stderr > $3.stderr.filter ; \
else \
	touch $3.stderr.filter ; \
fi

diff $3.stderr.filter $3.test.filter 2>&1 | cat > $3.diff

# first-try: if different, see if it's because of bison...
# if so, re-run using bison's expected output
# otherwise, exit with error
if [ -s $3.diff ] ; then \
	if grep ^YACC Makefile | grep -q bison ; then \
		if [ -f $2/$3.stderr.bison ] ; then \
			$4 $2/$3.stderr.bison > $3.stderr.bison.filter ; \
		else \
			touch $3.stderr.bison.filter ; \
		fi ; \
		diff $3.stderr.bison.filter $3.test.filter 2>&1 | cat > $3.bison.diff ; \
		if ! [ -s $3.bison.diff ] ; then \
			mv -f $3.bison.diff $3.diff ; \
		else \
			echo "$3.bison.diff is non-empty!"; \
		fi
	fi ; \
fi

if [ -s $3.diff ] ; then \
	echo "$3.diff is non-empty!  See $5."; \
	echo `pwd`/"$3.diff" >> $5; \
	exit 1; \
fi

