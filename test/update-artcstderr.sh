#!/bin/sh
# "update-artcstderr.sh"
# convenient maintainer script for updating expected outputs
# doesn't deal with bison stderr files yet

for i in $@
do
	cp -f $i.test $i.stderr
	echo "Updated $i.stderr"
done

