#!/bin/sh
# "test/getopt_test.sh"
# Produces expected output for testing libc getopt behavior.  
#	$Id: getopt_test.sh,v 1.1.2.1 2005/08/04 12:47:30 fang Exp $

cmd=getopt_test
redirin=/dev/null

$cmd < $redirin
$cmd foo < $redirin
$cmd -a < $redirin
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

