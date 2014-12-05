#!/bin/sh -e
# spice_to_pdf.sh
# spice netlist visualizer

# home=`dirname $0`

for d
do
t=`basename $d`
spice_line_join.awk $d | spice_to_dot.awk > $t.dot
dot -Tpdf $t.dot -o $t.pdf
echo "Created $t.pdf."
done

