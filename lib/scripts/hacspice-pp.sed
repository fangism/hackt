#!/usr/bin/sed -f

s/^[cC]/.capacitor /
s/^[dD]/.diode /
s/^[eE]/.vcvs /
s/^[fF]/.cccs /
s/^[gG]/.vccs /
s/^[hH]/.ccvs /
s/^[iI]/.ics /
s/^[jJ]/.jfet /
s/^[kK]/.mutualinductor /
s/^[lL]/.inductor /
s/^[mM]/.mosfet /
s/^[rR]/.resistor /
s/^[xX]/.instance /
s/^[sS]/.vcsw /
s/^[tT]/.transmissionline /
s/^[vV]/.ivs /
s/^[wW]/.ccsw /
s/^[zZ]/.mesfet /
/^\*/d

