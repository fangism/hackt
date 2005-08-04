#!/bin/sh
# "update-artcstderr.sh"
# convenient maintainer script for updating expected outputs
# doesn't deal with bison stderr files yet

# TODO: make update conditional on yacc/bison configuration?
# too much hassle because not all parse failure outputs are 
# dependent on the parser-generator

for i in $@
do
	cp -f $i.test $i.stderr
	echo "Updated $i.stderr"
done

