#! /usr/bin/awk -f
#	$Id: hackt-parse-expect-filter.awk,v 1.2 2005/11/10 02:13:12 fang Exp $
# "test/hackt-parse-expect-filter.awk"
# combines the work in "state_enum_filter.awk" and "vpath_file_filter.awk"
# This script just calls library functions defined in other files.


# dependencies: state_enum_filter.awk, vpath_file_filter.awk

# for all lines
{
	print filter_state_enums(filter_vpath_files($0));
}

