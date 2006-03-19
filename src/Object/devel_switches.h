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
	$Id: devel_switches.h,v 1.10.2.2 2006/03/19 06:14:08 fang Exp $
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
	Define to 1 to use a simple nonmeta instance reference class
	hierarchy.  
	Goal: 1
 */
#define	SIMPLIFY_NONMETA_INSTANCE_REFERENCES		1

/**
	Define to 1 to use type_equivalence between nonmeta references
	and expressions, instead of get_data_type_ref.  
	Goal: now 0 (unchanged), was 1 (new, afer having second thoughts)
 */
#define	NONMETA_TYPE_EQUIVALENCE			0

/**
	Define to 1 to use new (hopefully improved) nonmeta instance
	reference class hierarchy.  
 */
#define	NEW_NONMETA_REFERENCE_HIERARCHY	(1 && SIMPLIFY_NONMETA_INSTANCE_REFERENCES)

//=============================================================================

#endif	// __HAC_OBJECT_DEVEL_SWITCHES_H__

