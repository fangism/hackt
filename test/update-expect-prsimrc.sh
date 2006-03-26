#!/bin/sh
# "update-expectprs.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.prsimrc-out $i.prsimrc-expect
	echo "Updated $i.prsimrc-expect"
done

