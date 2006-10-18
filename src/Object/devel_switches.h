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
	$Id: devel_switches.h,v 1.24 2006/10/18 19:07:52 fang Exp $
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
	Define to 1 to enable lookup of global meta parameter values.
	Affects "Object/unroll/unroll_context_value_resolver.cc".
	Goal: to be decided by language spec.
	NOTE: 20060-09-10
	This may no longer be an issue once all values are resolved
		through footprints with the switch below.  
 */
#define	LOOKUP_GLOBAL_META_PARAMETERS		1

/**
	Replace instance collection replications with placeholders, 
	and use back-references to the placeholders.  
	Goal: 1
 */
#define	USE_INSTANCE_PLACEHOLDERS		1

/**
	All values, including template parameters shall be resolved
	through footprints.  
	Unroll_context will no longer use template_formals and actuals
	directly.  
	Goal: 1
	Status: still undergoing testing as development continues
 */
#define	RESOLVE_VALUES_WITH_FOOTPRINT	(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 0 to remove formal instance management (template, port)
	from the main body of sequential unrolling.  
	Rationale: remove duplicates
	Goal: 0
	Status: applied to templates, haven't applied to ports yet
 */
#define	SEQUENTIAL_SCOPE_INCLUDES_FORMALS	0

/**
	Define to 1 to distinguish target (instantiation) footprints
	from lookup-only footprints in the unroll_context.
	Rationale: easy distinction between instantiation and lookup.
	This needs to be done before we chain contexts to prevent
	accidental instantiation in the wrong context.  
	Goal: 1
	Status: complete, just needs complete testing
 */
#define	SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS	(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 to distinguish lvalue from rvalue lookups at unroll time.
	Rationale: global (out-of-scope) values may be only referenced 
		read-only, while local (target footprint) values may be
		modified.  
	Goal: 1
	Status: looks good so far, awaiting complete testing
 */
#define	RVALUE_LVALUE_LOOKUPS	(1 && SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS)

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

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

