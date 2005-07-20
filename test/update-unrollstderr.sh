#!/bin/sh
# "update-unrollstderr.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.unrolldump.filter $i.unrollstderr
	echo "Updated $i.unrollstderr"
done

