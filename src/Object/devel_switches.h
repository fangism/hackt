/**
	\file "Object/devel_switches.h"
	Module-wide preprocessor conditionals.
	Use of this file (ideally) should be confined to the Object directory.  

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.30.2.11 2006/11/06 20:40:43 fang Exp $
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
	TODO: decide policy for handling relaxed type actuals.  
 */

/**
	Define to 0 to remove formal instance management (template, port)
	from the main body of sequential unrolling.  
	Rationale: remove duplicates
	Goal: 0
	Status: applied to templates, haven't applied to ports yet
 */
#define	SEQUENTIAL_SCOPE_INCLUDES_FORMALS	0

/**
	Define to 1 to use dynamic_meta_index_list instead of
	abstract meta_index_list, again because we don't need
	compile-time static analysis anymore.
	Goal: 1
	Status: not begun
 */
#define	ALWAYS_USE_DYNAMIC_INDEX_LIST		0

/**
	Define to 1 to allow the parse_context class to support 
	nested outstanding definitions.  
	Useful for allowing top-level scope to be considered a
	definition, where module <= process_definition.  
	Also allows future support for nested definitions. 
	Goal: 1
	Status: complete, basically tested.
 */
#define	SUPPORT_NESTED_DEFINITIONS	1

/**
	Define to 1 to introduce and use dense formal value collections.
	Not critical since they are not replicated much, 
	may help performance in lookup a bit, save a little memory
	from sparse structures.  
	Goal: 1
	Status: not begun
	Priority: low, not memory critical
 */
#define	DENSE_FORMAL_VALUE_COLLECTIONS		0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to remove super_instance pointers from 
	instance_collection_base, and push them down to 
	port_actual_collections only.  
	Rationale: to save some memory on structures that don't need it.  
	Goal: 1?
	Status: not begun
	Priority: low, slightly memory critical
 */
#define	SUPER_INSTANCES_IN_ACTUALS_ONLY		0

/**
	Define to 1 to instantiate port_actual_collections. 
	Nothing more specific about HOW they are used is implied by
	this switch.  
	Goal: 1
	Status: completed, tested
 */
#define	ENABLE_PORT_ACTUAL_COLLECTIONS		1

/**
	Define to 1 to *use* port_actual_collections in subinstances.  
	Doesn't specify how they are allocated, see next flags.  
	Goal: 1
	Status: completed, tested, using heap allocation
 */
#define	ALLOCATE_PORT_ACTUAL_COLLECTIONS	(1 && ENABLE_PORT_ACTUAL_COLLECTIONS)

/**
	Define to 1 to pool-allocate ALL instance collections, on a 
	*per-footprint* basis (using the target footprint in context).  
	This is really orthogonal to whether or not port_actual_collections
	are used.  
	Goal: 1?
	Status: complete and tested
 */
#ifndef	POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
#define	POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT	1
#endif

/**
	Define to 1 to heap-allocate footprints. 
	Rationale, makes persistence and back-referencing easier.
	Also footprint_manager.h need not include footprint.h, 
	which carries a lot of weight.  
	Footprints are not replication critical.  
	UPDATE: this will be needed because of footprint-dependent 
		de-serialization ordering!  port_actual_collections'
		have back-references to formal_collections that belong to
		other footprints!
	Can also pool-allocate footprints.
	Goal: 1
	Status: complete, and tested
 */
#define	HEAP_ALLOCATE_FOOTPRINTS		1

/**
	Define to 1 to reference-count footprints. 
	Don't know if this is necessary, or a good idea, but it is an option.
	Purpose: Could use this to validate acylic dependencies...
	Goal: ?
	Status: not begun
 */
#if	HEAP_ALLOCATE_FOOTPRINTS
#define	REF_COUNT_FOOTPRINTS			0
#endif

/**
	Define to 1 to employ locks to check acyclic dependencies 
	between footprints.  
	Goal: 1
	Status: not begun
 */
#define	FOOTPRINT_LOCKS				0

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

