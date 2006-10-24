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
	$Id: devel_switches.h,v 1.29.2.7 2006/10/24 05:16:10 fang Exp $
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
 */
#define	DENSE_FORMAL_VALUE_COLLECTIONS		0

/**
	Define to 1 to have port collections use light-weight back-references
	to footprint's internal collection maps.  
	This way port instance collections don't need to contain
	collection mapping information, just refer to footprint.  
	Goal: 1?
	Status: not begun
 */
#define	PORT_COLLECTIONS_USE_BACK_REFERENCE	0

/**
	Define to 1 to use per-module allocated pools of subinstance lists.
	Purpose: turn individually heap-allocated structures into
		a pseudo-dense collection, referenced by indices.  
	Affects: instance_alias_info's substructure, subinstance_manager.
	Goal: ?
	Status: not begun
 */
#define	MODULE_POOLED_SUBINSTANCE_STRUCTURE	0

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

