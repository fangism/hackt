#! /usr/bin/awk -f
#	$Id: hackt-parse-expect-filter.awk,v 1.1.2.1 2005/11/10 00:47:48 fang Exp $
# "test/hackt-parse-expect-filter.awk"
# combines the work in "state_enum_filter.awk" and "vpath_file_filter.awk"
# This script just calls library functions defined in other files.


# dependencies: state_enum_filter.awk, vpath_file_filter.awk

# for all lines
{
	print filter_state_enums(filter_vpath_files($0));
}

