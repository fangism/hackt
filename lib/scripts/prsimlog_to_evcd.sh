#!/bin/sh -e
#	"prsimlog_to_evcd.sh"
#	$Id: prsimlog_to_evcd.sh,v 1.1 2009/08/28 22:41:50 fang Exp $

# usage (result printed to stdout):
# $0 file > evcd
# TODO: forward options

# expects the following scripts in path:
# prsimlog_to_evcd_ports.awk
# prsimlog_to_evcd.awk

log=$1
temp=.$1.ports
date=`date`

cat <<EOF
\$date
    $date
\$end
\$timescale
    1ps
\$end
\$comment generated by prsimlog_to_evcd.{sh,awk} \$end

EOF
#\$version
#    Chronologic Simulation VCS Release Y-2006.06-SP1
#    \$dumpports(PRSIM, "prsim.evcd")
#\$end

prsimlog_to_evcd_ports.awk $log > $temp
prsimlog_to_evcd.awk -v port_file=$temp $log
rm -f $temp
