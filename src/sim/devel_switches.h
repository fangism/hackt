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
	$Id: devel_switches.h,v 1.3 2006/05/06 04:18:44 fang Exp $
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
	Define to 1 to track node-to-node cause paths by direction, 
	so we can track complete cycles in backtracing critical paths.  
	Goal: 1
 */
#define	SEPARATE_CAUSE_NODE_DIRECTION		0

//=============================================================================

#endif	// __HAC_SIM_DEVEL_SWITCHES_H__

