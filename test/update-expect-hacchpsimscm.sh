#!/bin/sh
# "update-expect-hacchpsimscm.sh"
# convenient maintainer script for updating expected outputs

args=`getopt "s:" $*`
if test $? != 0
then
	echo "Usage: $0 [ -s srcdir ] [files...]"
	exit 2
fi
set -- $args

# assumed source directory
srcdir=.
for i
do
	case "$i"
	in
		-s) srcdir="$2" shift; shift;;
		--) shift; break;;
	esac
done

for i
do
	if test -f $i.hacchpsimscm-out
	then
		cp -f $i.hacchpsimscm-out $srcdir/$i.hacchpsimscm-expect
		echo "Updated $srcdir/$i.hacchpsimscm-expect"
	elif test -f $i.hacchpsimscmfail-out
	then
		cp -f $i.hacchpsimscmfail-out $srcdir/$i.hacchpsimscm-expect
		echo "Updated $srcdir/$i.hacchpsimscm-expect"
	else
		echo "Missing $i.hacchpsimscm[fail]-out"
	fi
done

