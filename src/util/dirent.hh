/**
	\file "util/dirent.hh"
	Configured version of <dirent.h> and <sys/dirent.h>.
	$Id: dirent.hh,v 1.2 2005/11/10 02:13:10 fang Exp $
 */

#ifndef	__UTIL_DIRENT_H__
#define	__UTIL_DIRENT_H__

#include "config.h"

#if defined(HAVE_DIRENT_H) && HAVE_DIRENT_H
#include <dirent.h>
#elif defined(HAVE_SYS_DIRENT_H) && HAVE_SYS_DIRENT_H
#include <sys/dirent.h>
#endif

namespace util {
// we provide a few useful wrapper functions to minimize #if configure-clutter

extern
bool
dir_exists(const char*);

}	// end namespace util

#endif	// __UTIL_DIRENT_H__
