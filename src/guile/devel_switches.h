/**
	\file "guile/devel_switches.h"
	Use of this file (ideally) should be confined to the guile directory.  

	$Id: devel_switches.h,v 1.1.2.1 2007/04/05 01:04:50 fang Exp $
 */

#ifndef	__HAC_GUILE_DEVEL_SWITCHES_H__
#define	__HAC_GUILE_DEVEL_SWITCHES_H__

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
/**
	Define to 1 to use symbols instead of constant variables:
	replace (define bool-tag 13) with 'bool-tag instead.
	Rationale: symbols are uniquely allocated objects, ints are replicated.
	Goal: 1
	Status: done, tested, ready to perm.
 */
#define	SCM_USE_SYMBOLIC_TYPE_TAGS				1

//=============================================================================

#endif	// __HAC_GUILE_DEVEL_SWITCHES_H__

