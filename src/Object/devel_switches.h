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
	$Id: devel_switches.h,v 1.1.2.2 2005/08/15 19:58:27 fang Exp $
 */

#ifndef	__OBJECT_DEVEL_SWITCHES_H__
#define	__OBJECT_DEVEL_SWITCHES_H__

// if we want project-wide switches
#if 0
#include "common/devel_switches.h"
#endif

// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  
#endif

// define your project-wide development switches here:

//=============================================================================
// The switches in this section should expire by the end of the
//	ARTXX-00-01-04-main-00-55-footprint-01-05-canonical branch:
#define	USE_CANONICAL_TYPE				1

//=============================================================================

#endif	// __OBJECT_DEVEL_SWITCHES_H__

