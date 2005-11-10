/**
	\file "util/dirent.cc"
	Wrapped functions related to directories.  
	$Id: dirent.cc,v 1.2 2005/11/10 02:13:10 fang Exp $
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
		assert(!e);		// exist status OK
	}
	return ret;
}

//=============================================================================
}	// end namespace util

