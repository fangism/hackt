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
	$Id: devel_switches.h,v 1.1.4.12 2005/09/04 18:10:42 fang Exp $
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

/**
	Upgrade port_formals and subinstance_manager members to
	physical_instance_collections instead of instance_collection_base.
	Goal: 1
 */
#define	PHYSICAL_PORTS				1
/// Goal: 1
#define	CREATE_DEPENDENT_TYPES_FIRST		1

// #define	USE_NEW_REPLAY_INTERNAL_ALIAS	1 && CREATE_DEPENDENT_TYPES_FIRST
//=============================================================================

#endif	// __OBJECT_DEVEL_SWITCHES_H__

