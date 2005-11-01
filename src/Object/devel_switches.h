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
	$Id: devel_switches.h,v 1.5.4.2 2005/11/01 04:23:55 fang Exp $
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
	Whether or not to use a compute-once-on-demand string cache, 
	or re-evaluate strings each time during cflat traversal.  
	Not only is this an optimization, but this will be needed
	to re-work cflat aliase to fix some bugs.  
	Goal: 1
	Status: stable, ready to commit
 */
#define	USE_ALIAS_STRING_CACHE			1

/**
	Whether or not to track parent back references during 
	unique object allocation phase.  
	Will be useful for new (correct) cflat algorithm.  
	Goal: undecided
 */
#define USE_GLOBAL_ENTRY_PARENT_REFS            1

//=============================================================================

#endif	// __OBJECT_DEVEL_SWITCHES_H__

