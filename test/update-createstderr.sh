#!/bin/sh
# "update-createstderr.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.createdump.filter $i.createstderr
	echo "Updated $i.createstderr"
done

