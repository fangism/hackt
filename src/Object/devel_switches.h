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
	$Id: devel_switches.h,v 1.18.2.6 2006/10/02 03:18:55 fang Exp $
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
	Status: still undergoing testing as development continues
 */
#define	RESOLVE_VALUES_WITH_FOOTPRINT	(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 0 to disable compile-time (pre-unroll) analysis
	of expressions and value references.  
	Goal: 0
	Status: stable, committable up-branch.
 */
#define	ENABLE_STATIC_ANALYSIS		(0 && !USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 to derive module from process_definition, 
	re-using many of the facilities already available.  
	Goal: 1
	Status: so far unroll phase testing and regression fixing has begun, 
		haven't started create-phase work yet.  
 */
#define	MODULE_PROCESS			(1 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 to reference-count instance-management statements.  
 */
#define	REF_COUNT_INSTANCE_MANAGEMENT	1

/**
	Define to 1 to put instantiation statements *owned* (as opposed 
	to just referenced) in the placeholders themselves.  
	Rationale: avoid duplicate instantiation without hacking in
	special-case code, also a simplification.  
	Need this for forward calls to get_unresolved_type_ref().  
	NOTE: the placeholders and instantiation statements cyclicly
		reference each other (not a problem).  
	Problem: excl_ptr (ports) vs. never_ptr (locals)?
	Prerequisite solution: 
		reference-count instance-management statements (above)
		Back-reference to placeholder are always weak (never_ptr).
	Goal: ?
	Status: experimental
 */
#define	PLACEHOLDERS_OWN_INSTANTIATIONS	(0 && USE_INSTANCE_PLACEHOLDERS)

/**
	Define to 1 for placeholders to *include* port indices 
	instead of including a reference to an unroll statement.
	At unroll time, an auxiliary unroll statement will be formed
	and processed.  
	This also means port instantiations will not be added to sequential
	scopes, but rather, implicitly unrolled at the ports.  
	Rationale: code simplification, and reduces duplicate
		instantiation of ports.  
	Goal: ?
	Status: experimental
 */
#define	PLACEHOLDER_PORT_INDICES	(0 && USE_INSTANCE_PLACEHOLDERS)

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

