#!/bin/sh -e
# no options
# converts spice file to type-dependency graph

for f
do
	printf "Generating graph for $f-pdf ..."
	spice_type_deps.awk $f | sort -u | deps_to_dot.awk > $f-dot
	dot -Tpdf $f-dot -o $f-pdf
	echo " done."
done

