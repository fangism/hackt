/**
	\file "sim/devel_switches.h"
	Simulator-wide preprocessor conditionals.
	Use of this file (ideally) should be confined to the sim/ directory.  

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.2.6.3 2006/05/04 02:51:36 fang Exp $
 */

#ifndef	__HAC_SIM_DEVEL_SWITCHES_H__
#define	__HAC_SIM_DEVEL_SWITCHES_H__

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
// define your simulator-wide development switches here:

/**
	Define to 1 to use reconstruction algorithm for 
	restoring intermediate expression state from checkpoint.
	Little slower, but dramatically reduces checkpoint size.
	Goal: 1
	Status: looks good, checkpoint consistency tests are satisfactory.  
 */
#define	DEDUCE_PRSIM_EXPR_STATE			1

/**
	Define to 1 to enable prsim expression optimization, 
	where literal leaf nodes are folded.  
	This results in a siginificant reduction in the number of expression 
	leaf nodes if nodes fanout directly to expressions as operands.  
	NOTE: this transformation is only legal when there is no information
	to be retained per literal.  e.g. sized PRS cannot use this.  
	Goal: 1
 */
#define	PRSIM_ENABLE_LITERAL_FOLDING		1

/**
	Define to 1 to allow expression trees to be rewritten
	by pulling negations closer to roots.  
	This sometimes results in shorter expression paths.  
	Goal: 1
 */
#define	PRSIM_ENABLE_NEGATION_DENORMALIZATION	1

//=============================================================================

#endif	// __HAC_SIM_DEVEL_SWITCHES_H__

