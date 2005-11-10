/**
	\file "util/file_status.h"
	Convenience structure for determining result of opening a file.
	$Id: file_status.h,v 1.2 2005/11/10 02:13:11 fang Exp $
 */

#ifndef	__UTIL_FILE_STATUS_H__
#define	__UTIL_FILE_STATUS_H__

#include <cstdio>

namespace util {
//=============================================================================
/**
	Convenient paired return type include a FILE* and 
	a status enumeration.  
	Consider using a set of flags in future revisions.  
	Closing the file is responsibility of the caller
	and is not handled here.  
 */
struct file_status {
	typedef enum {
		NOT_FOUND,              ///< named file was not found
		NEW_FILE,               ///< file opened is first time
		SEEN_FILE,              ///< file was seen before
		CYCLE                   ///< file is already open on stack
	} status;

	FILE*				file;
	status				stat;

	file_status() : file(NULL), stat(NOT_FOUND) { }
	file_status(FILE* f, status s) : file(f), stat(s) { }

	operator bool () const { return file && stat != NOT_FOUND; }

};	// end struct file_status

//=============================================================================
}	// end namespace util

#endif	// __UTIL_FILE_STATUS_H__

