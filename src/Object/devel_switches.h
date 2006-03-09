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
	$Id: devel_switches.h,v 1.9.4.1 2006/03/09 05:50:22 fang Exp $
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
	Goal: 0
 */
#define INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST       1

/**
	Preparing to make major cuts, carving out ring_node structures...
	Mark cuts with this preprocessor conditional.  
	Goal: 0, replace ring_nodes with union_finds.  
	Once this is done, we should be able to eliminate
	INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST, above.  
 */
#define	USE_ALIAS_RING_NODES				0

/**
	Split the create phase into two subphases:
	a) replay internal aliases of each referenced alias.
		i) this could be made more efficient... see flag below.
	b) after all connections finalized, visit all public aliases and assign
		instance_id's by following union-finds.  
	Goal: 1
 */
#define	SEPARATE_ALLOCATE_SUBPASS		(1 && !USE_ALIAS_RING_NODES)

/**
	Rework the create phase to NOT use the same traversal as unrolling.
	Instead, walk over all instance collections in each scope.
	For each alias in each collection, replay internal aliases.  
	This should result in a more efficient create pass.  
	Goal: 1
 */
#define	INSTANCE_BASED_CREATE_PHASE			1

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

