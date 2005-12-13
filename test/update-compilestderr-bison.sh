#!/bin/sh
# "update-compilestderr-bison.sh"
#	$Id: update-compilestderr-bison.sh,v 1.2 2005/12/13 04:15:53 fang Exp $
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

