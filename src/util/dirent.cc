/**
	\file "util/dirent.cc"
	Wrapped functions related to directories.  
	$Id: dirent.cc,v 1.3 2006/02/13 02:48:06 fang Exp $
 */

#include <cassert>
#include "util/dirent.h"

namespace util {
//=============================================================================
/**
	\return true if the directory exists.  
 */
bool
dir_exists(const char* d) {
	DIR* const dir = opendir(d);
	const bool ret = dir;		// true if valid
	if (dir) {
		const int e = closedir(dir);
		assert(!e);		// exit status OK
		return !e;
	}
	return ret;
}

//=============================================================================
}	// end namespace util

