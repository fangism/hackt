#!/bin/sh -e
# translates ACT's old prs syntax to HAC's new syntax
#	$Id: hac_fix_vt.sh,v 1.1 2010/05/19 23:09:31 fang Exp $
# usage:
# $0 < .act > .hac
# maintainer: Fang

# This change was introduced in the tools:
# 2009-09-14      David Fang              <fangism@users.sourceforge.net>
#	* HACKT-00-01-04-main-00-83-29-merged-netlist-02-13:
detect_compat ()
{
vers=`hackt-config --version | sed 's/^.*-//'`

if test $vers -gt 20090915
then fwd=1 ; bkwd=0
else fwd=0 ; bkwd=1
fi
}

usage ()
{
cat <<EOF
hac_fix_vt.sh: forward/backward compatibiity filter for ACT/HAC
usage: $0 [options] < in > out
options:
  -h : help
  -a : automatic detection of version (default)
  -b : force backward compatibility
  -f : force forward compatibility
  -n : disable compatibility filters
EOF
}

# default
detect_compat

# process options
for f
do
case $f in
	-h ) usage ; exit ;;
	# force forward compatibility
	-f ) fwd=1 ; bkwd=0 ;;
	# force backward compatibility
	-b ) fwd=0 ; bkwd=1 ;;
	# automatic (default)
	-a ) detect_compat ;;
	-n ) fwd=0 ; bkwd=0 ;;
	* ) echo "Error argument '$f'." ; usage ; exit 1 ;;
esac
done

# run
if test $fwd = 1 ; then
# transformation for forward compatibility
# for now we delete it until HAC knows how to handle it
# -e 's|,[ ]*od18|;od18|g'
exec sed \
-e 's|,[ ]*od18||g' \
-e 's|,[ ]*hvt|;hvt|g' \
-e 's|,[ ]*svt|;svt|g' \
-e 's|,[ ]*lvt|;lvt|g'
elif test $bkwd = 1 ; then
# reverse transformation for backward compatibility
exec sed \
-e 's|;[ ]*od18|,od18|g' \
-e 's|;[ ]*hvt|,hvt|g' \
-e 's|;[ ]*svt|,svt|g' \
-e 's|;[ ]*lvt|,lvt|g'
else
exec cat
fi

