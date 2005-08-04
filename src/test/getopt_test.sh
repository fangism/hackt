#!/bin/sh
# "test/getopt_test.sh"
# Produces expected output for testing libc getopt behavior.  
#	$Id: getopt_test.sh,v 1.1.2.3 2005/08/04 20:14:18 fang Exp $

cmd=getopt_test
redirin=/dev/null

$cmd < $redirin
$cmd foo < $redirin
$cmd foo bar < $redirin
$cmd -a < $redirin
$cmd -aa < $redirin
$cmd -a -a < $redirin
$cmd -b < $redirin
$cmd -ab < $redirin
$cmd -a -b < $redirin
$cmd -c < $redirin
$cmd -c foo < $redirin
$cmd -cfoo < $redirin
$cmd -ac foo < $redirin
$cmd -acfoo < $redirin
$cmd -cbfoo < $redirin
$cmd -cb foo < $redirin
$cmd -c -bfoo < $redirin
$cmd -c -b foo < $redirin
$cmd -bfoo < $redirin
$cmd -b foo < $redirin
$cmd -b foo bar ick < $redirin
$cmd -b foo -a bar ick < $redirin
$cmd -b foo -b bar ick < $redirin
$cmd -c foo -b bar ick < $redirin
$cmd -c foo -c bar ick < $redirin
$cmd -b foo -c bar ick < $redirin
$cmd -x < $redirin
$cmd -x foo < $redirin
$cmd -xy < $redirin

