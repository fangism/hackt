#!/bin/sh
# "multi_pool_module_test.morefilter.sh"
#	$Id: multi_pool_module_test.morefilter.sh,v 1.1.4.1 2005/01/27 23:36:19 fang Exp $

# $1 is the root of the file name to filter, which must exist
# $2 is the name of the target file

source=$1
target=$2

if ! [ -f $source ]
then
	echo "Missing $source"
	exit 1
fi

# any sed/awk madness goes here

sed '/\\- MAIN/,/\/- MAIN/!d' $source > $target

