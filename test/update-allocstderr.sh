#!/bin/sh
# "update-allocstderr.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.allocdump.filter $i.allocstderr
	echo "Updated $i.allocstderr"
done

