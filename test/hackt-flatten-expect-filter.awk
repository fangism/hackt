#! /usr/bin/awk -f
#	$Id: hackt-flatten-expect-filter.awk,v 1.1 2006/11/15 00:09:22 fang Exp $
# "test/hackt-flatten-expect-filter.awk"
# combines the work in "state_enum_filter.awk" and "vpath_file_filter.awk"
# This script just calls library functions defined in other files.

# dependencies: vpath_file_filter.awk

# for all lines
{
	print flatten_filter_vpath_files($0);
}

