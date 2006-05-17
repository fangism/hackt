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
	$Id: devel_switches.h,v 1.13.4.3 2006/05/17 02:22:44 fang Exp $
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
	Define to 1 to enable nonmeta_instance_references to
	meta parameters, (pint, pbool, preal).  
	These instance references, however, will be rvalue-only.  
	Compile-time constants can never be lvalues!
	Goal: 1
 */
#define	USE_NONMETA_VALUE_REFERENCES			1

/**
	Define to 1 to enable CHP footprint unrolling.  
	Goal: 1
 */
#define	ENABLE_CHP_FOOTPRINT				1

/**
	Define to 1 to implement copy-on-write nonmeta-expression
	unrolling.  
	Copy-on-write will result in a reference copy when result is
	unchanged, and a fresh deep copy if result is changed.  
	Wish: covariant return types.  (g++-3.4 and newer)
	Goal: 1
 */
#define	COW_UNROLL_DATA_EXPR				1

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

