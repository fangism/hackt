#!/bin/sh
# "update-artcstderr.sh"
#	$Id: update-artcstderr-bison.sh,v 1.1.2.1 2005/11/06 21:55:06 fang Exp $
# convenient maintainer script for updating expected outputs
# use this script when updating bison tests

# TODO: make update conditional on yacc/bison configuration?
# too much hassle because not all parse failure outputs are 
# dependent on the parser-generator

for i in $@
do
	cp -f $i.test.filter $i.stderr.bison
	echo "Updated $i.stderr.bison"
done

