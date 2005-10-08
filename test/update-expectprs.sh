#!/bin/sh
# "update-expectprs.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.prs $i.expect-prs
	echo "Updated $i.expect-prs"
done

