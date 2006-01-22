#!/bin/sh
# "update-expectprsimexpr.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.prsimexpr $i.expect-prsimexpr
	echo "Updated $i.expect-prsimexpr"
done

