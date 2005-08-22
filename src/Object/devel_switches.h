/**
	\file "Object/devel_switches.h"
	Module-wide preprocessor conditionals.
	Use of this file (ideally) should be confied to the Object directory.  

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.1.4.6 2005/08/22 19:59:32 fang Exp $
 */

#ifndef	__OBJECT_DEVEL_SWITCHES_H__
#define	__OBJECT_DEVEL_SWITCHES_H__

// if we want project-wide switches
#if 0
#include "common/devel_switches.h"
#endif

// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  \
	However, if you are developing, use this file as you see fit.  
#endif

//=============================================================================
// define your module-wide development switches here:

#define	USE_UNROLL_CONTEXT_FOOTPRINT		1

// done, ready to commit a while ago
#define	COPY_CONTEXT_ACTUALS			1

//=============================================================================

#endif	// __OBJECT_DEVEL_SWITCHES_H__

