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
	$Id: devel_switches.h,v 1.18.2.4.2.1.2.1 2006/09/28 22:37:19 fang Exp $
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
	Define to 1 to distinguish get_type() between unresolved
	and resolved variants.  Unresolved types are deduced from
	the first instantiation statement associated with a 
	particular placeholder, whereas resolved types (canonical)
	are resolved from context (+footprint) at the actual collection.  
	Depends on USE_INSTANCE_PLACEHOLDERS.
	Goal: 1
 */
#define	USE_RESOLVED_DATA_TYPES		(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 if we want to be able to infer the fundamental
	(pre-unroll-resolved) data type of a nonmeta data expression.
	This is used to type-check (nonmeta) CHP expressions.  
	This option is not exclusive with USE_RESOLVED_DATA_TYPES.  
	Without this, check is just deferred until unroll-time.  
	Goal: 1
	Status: 'probably' OK to keep, but will wait until testing
 */
#define	USE_UNRESOLVED_DATA_TYPES	(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	All values, including template parameters shall be resolved
	through footprints.  
	Unroll_context will no longer use template_formals and actuals
	directly.  
	Goal: 1
 */
#define	RESOLVE_VALUES_WITH_FOOTPRINT	(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 0 to disable compile-time (pre-unroll) analysis
	of expressions and value references.  
	Goal: 0
 */
#define	ENABLE_STATIC_ANALYSIS		(0 && !USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 to derive module from process_definition, 
	re-using many of the facilities already available.  
	Goal: 1
 */
#define	MODULE_PROCESS			(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 to give every definition type a footprint
	and/or footprint_manager.
	Rationale: since all parameter unrollings (local and formal)
	now take place in a footprint, this will make functions
	more self-consistent.  
	Goal: 1?
 */
#define	DEFINITION_FOOTPRINTS		(0 && USE_INSTANCE_PLACEHOLDERS)

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

