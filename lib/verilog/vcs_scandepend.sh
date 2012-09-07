#!/bin/sh -e
# "vcs_scandepend.sh"
#	$Id: vcs_scandepend.sh,v 1.1 2009/10/22 22:30:28 fang Exp $
# scans VCS compile log for included source files to list as dependencies

exec grep -e "Parsing included file" -e "Parsing design file" | \
sed -e "s|^[^']*'|  |" -e "s|'[^']*$| \\\\|" -e '$s| \\||'

