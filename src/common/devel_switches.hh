/**
	\file "common/devel_switches.h"
	Project-wide preprocessor conditionals.

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.2 2005/09/04 21:15:05 fang Exp $
 */

#ifndef	__COMMON_DEVEL_SWITCHES_H__
#define	__COMMON_DEVEL_SWITCHES_H__

// leave this error enabled for released code
#if 1
#error	Production code should NOT include this header file. \
	If you are developing, however, feel free to disable this error \
	and define custom switches for testing purposes. 
#endif

// define your project-wide development switches here:

#endif	// __COMMON_DEVEL_SWITCHES_H__

