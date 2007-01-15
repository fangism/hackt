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
	$Id: devel_switches.h,v 1.37.2.8 2007/01/15 06:28:53 fang Exp $
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
	Priority: low (if it ain't broke...), matter of consistency
 */
#define	SEQUENTIAL_SCOPE_INCLUDES_FORMALS	0

/**
	Define to 1 to use dynamic_meta_index_list instead of
	abstract meta_index_list, again because we don't need
	compile-time static analysis anymore.
	Goal: 1
	Status: not begun
	Priority: low
	NOTE: can also replace const_param_expr_list-pointers with
	non-pointers to reduce heap-allocations.  
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
	This is now defined in config.h by configure.  
	Define to 1 to fuse unrolling and creating into the same phase.
	Rationale: with PROPAGATE_CHANNEL_CONNECTIONS_HIERARCHICALLY (perm'd), 
		we now have no use for an unrolled-but-not-created footprint.
	Goal: 1 (reluctantly)
	Status: complete
	Priority: medium-high
	Affects: top-level executable programs, internal passes 
		don't need to use this flag.  
 */
// #define	UNIFY_UNROLL_CREATE			1

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
	Define to 1 to pool-allocate collection pool bundles.
	Goal: 1
	Status: not begun
	Priority: low, non-critical enhancement
 */
#define	POOL_ALLOCATE_COLLECTION_POOL_BUNDLES		0

/**
	Define to 1 to pool-allocate footprints. 
	Prerequisite: Depends on footprints being heap-allocated, of course
	Rationale: faster heap allocation, of course.  
	Goal: 1
	Status: not begun
	Priority: low (enhancement, non-critical)
 */
#define	POOL_ALLOCATE_FOOTPRINTS		0

/**
	Define to 1 to reference-count footprints. 
	Don't know if this is necessary, or a good idea, but it is an option.
	Purpose: Could use this to validate acylic dependencies...
	Prerequisite: heap-allocating footprints (done)
	Goal: ?
	Status: not begun
 */
#define	REF_COUNT_FOOTPRINTS			0

/**
	Define to 1 to employ locks to check acyclic dependencies 
	between footprints.  
	Goal: 1
	Status: not begun
	Priority: low-medium, for stronger invariant checking.  
 */
#define	FOOTPRINT_LOCKS				0

/**
	Define to 1 to use footprint sub-types, distinguishing between
	process, datastruct, and channel footprints.  
	Currently footprint is generic union of needed features, bloated.  
	Rationale: memory reduction and stronger static typing.  
	Goal: ?
	Priority: low
 */
#define	SUBTYPE_FOOTPRINTS			0

/**
	Define to 1 to instantiate struct collections and references, 
	which as of the time of writing this, have never been well-defined
	or used.  
	Goal: ?
	Pending: well-defined semantics in language
 */
#define	ENABLE_DATASTRUCTS			0

/**
	Define to 1 to re-define global_entry<channel> using
	canonical_fundamental_channel footprints.  
	Also allocate ChannelData accordingly.  
	Goal: 1
	Status: done
	Priority: high (for chpsim)
 */
#define	BUILTIN_CHANNEL_FOOTPRINTS		1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the below flags are done, revisit and perm them later

/**
	Define to 1 recurse collection of port aliases (redundantly redundant)
	Discovered that using instance_collection_pool_manager to collect
	instance aliases results in massive redundant repetition in
	footprint's scope_aliases (which is never dumper, incidentally).
	Disabling this behavior should result in smaller footprints.  
	Goal: ?
	Status: 1 (original)
	Resolution: we keep recursion for now, and revisit this later.  
 */
#define	RECURSE_COLLECT_ALIASES			1

/**
	Define to 1 to simplify implementation of collecting port aliases
	by collecting over scope-aliases in a separate pass.  
	Affects: footprint, port_alias_tracker.  
	Goal: 1
	Rationale: code reduction, maintainability
	Status: enabled and tested (perm. it later)
 */
#define	COPY_IF_PORT_ALIASES			1

/**
	Define to 1 to not serialize scope-alias summaries, but rather
	regenerate/recache them automatically and on-demand.  
	With this on, they are reconstructed upon footprint::load.  
	This cut the size of the object binaries, by reconstructing
		redundant (precisely regenerable) information.  
	Goal: 1? (probably)
	Priority: low-medium (memory intensive)
	Status: done, tested, but not perm'd yet.  
		Results in massive (factor of 2) savings on 
		large objects, especially top-level hierarchy.
 */
#define	AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES		1

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

