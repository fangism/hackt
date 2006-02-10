#!/bin/sh
# "update-expectsprs.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.sprs $i.expect-sprs
	echo "Updated $i.expect-sprs"
done

