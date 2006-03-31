#!/bin/sh
# "update-expect-prsimrc.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.prsimrc-out-filter $i.prsimrc-expect
	echo "Updated $i.prsimrc-expect"
done

