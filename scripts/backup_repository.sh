#!/bin/sh
#	$Id: backup_repository.sh,v 1.3 2006/01/22 18:55:56 fang Exp $
# Script is only useful to maintainer with local access to CVS repository.

# current directory
pwd=`pwd`

# module name
module=art++

# tar-ball base file name
base=$module-repository

# path to CVS repository (one up from root)
root=/ufs/repository/VLSI/tools

# date-time stamp suffix
ymdt=`date -u "+%Y%m%d-%H%M%S"`

# lcal file name
lfn=$base-$ymdt.tar.bz2

# full path to tar ball
bz=$pwd/$lfn
# echo $bz

cd $root && tar cjvf $bz $module

if [ -s $lfn ]
then
	echo "CVS repository backed up to $lfn"
	ls -l $lfn
else
	exit 1
fi

