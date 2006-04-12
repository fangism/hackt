#!/bin/sh
# "update-expectprsseu.sh"
# convenient maintainer script for updating expected outputs

for i in $@
do
	cp -f $i.prsseu $i.expect-prsseu
	echo "Updated $i.expect-prsseu"
done

