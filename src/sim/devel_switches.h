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
	$Id: devel_switches.h,v 1.2.6.1 2006/05/01 03:25:41 fang Exp $
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
	Define to 1 to enable prsim checkpointing capability.  
	Goal:
 */
#define	ENABLE_PRSIM_CHECKPOINT			1

//=============================================================================

#endif	// __HAC_SIM_DEVEL_SWITCHES_H__

