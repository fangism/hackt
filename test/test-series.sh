#!/bin/sh
# "test-series.sh"
# convenience script for running a sequence of tests on one input file.

# usage: pass in the root names of the test cases (without the .in extension)

for t in $@
do
	if ! [ -f $t.artctest ]
	then
		make $t.artctest
	fi
	echo $t.artctest
	$t.artctest
	if ! [ -s $t.test ]
	then
		if ! [ -f $t.artobjtest ]
		then
			make $t.artobjtest
		fi
		echo $t.artobjtest
		$t.artobjtest
		if ! [ -f $t.artobjunrolltest ]
		then
			make $t.artobjunrolltest
		fi
		echo $t.artobjunrolltest
		$t.artobjunrolltest
	fi
done


