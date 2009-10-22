#!/bin/sh -e
# "vcs_scandepend.sh"
#	$Id: vcs_scandepend.sh,v 1.1 2009/10/22 22:30:28 fang Exp $
# scans VCS compile log for included source files to list as dependencies

exec grep "Parsing included file" | \
sed -e "s|^[^']*'|  |" -e "s|'[^'].*$| \\\\|" -e '$s| \\||'

