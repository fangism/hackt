#!/bin/sh
# "multi_pool_module_test.morefilter.sh"
#	$Id: multi_pool_module_test.morefilter.sh,v 1.3 2005/12/08 22:01:14 fang Exp $

# $1 is the root of the file name to filter, which must exist
# $2 is the name of the target file

source=$1
target=$2

if test ! -f $source
then
	echo "Missing $source"
	exit 1
fi

# any sed/awk madness goes here

sed -e 's|(hex) 0$|(hex) ##ADDR##|' $source > $target

