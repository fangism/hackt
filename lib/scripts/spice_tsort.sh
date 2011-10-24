#!/bin/sh -ev
# reads .spice file from $1
# and topologically sorts type hierarchy from bottom-up
# with the final target listed last
# prints output to stdout (can be redirected)

for f
	do
# without assuming that .spice file is ordered properly
spice_line_join.awk $f | spice_type_deps.awk | sort -u | sed 's|:||' | awk '{print $2 " " $1;}' | tsort
# if you trust that .spice file is already ordered
# grep -i "^\.subckt" $f | cut -d\  -f2
done
