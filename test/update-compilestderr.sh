#!/bin/sh
# "update-compilestderr.sh"
#	$Id: update-compilestderr.sh,v 1.2 2005/12/13 04:15:54 fang Exp $
# convenient maintainer script for updating expected outputs
# use this script for updating yacc test outputs, not bison

# TODO: make update conditional on yacc/bison configuration?
# too much hassle because not all parse failure outputs are 
# dependent on the parser-generator

for i in $@
do
	cp -f $i.test.filter $i.stderr
	echo "Updated $i.stderr"
done

