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
	$Id: devel_switches.h,v 1.8.8.6 2006/02/18 06:28:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEVEL_SWITCHES_H__
#define	__HAC_OBJECT_DEVEL_SWITCHES_H__

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

/**
	Whether or not to apply the quick and dirty hack to
	fix a critical bug.  
 */
#define INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST       1

/**
	Define to 1 to use "new and improved" class hierarchy
	for instance references.  
	Goal: 1
 */
#define	DECOUPLE_INSTANCE_REFERENCE_HIERARCHY		1

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

