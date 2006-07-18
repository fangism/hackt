/**
	\file "sim/prsim/devel_switches.h"
	prsim preprocessor conditionals.
	Use of this file (ideally) should be confined to the 
	sim/prsim directory.  

	The sole purpose of this file is to provide a convenient
	place to include developer switches.  
	During development, it is often desirable to switch back and forth
	between two implementations and versions, controlled by a single
	preprocessor definition.  
	However, in production code, this file should be EMPTY, 
	and NO translation unit should depend on this i.e. do not include.  
	$Id: devel_switches.h,v 1.2 2006/07/18 04:09:17 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_DEVEL_SWITCHES_H__
#define	__HAC_SIM_PRSIM_DEVEL_SWITCHES_H__

// if we want project-wide switches
#if 0
#include "sim/devel_switches.h"
#endif

// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  \
	However, if you are developing, use this file as you see fit.  
#endif

//=============================================================================
// define your simulator-wide development switches here:

/**
	Define to 1 to track node-to-node cause paths by direction, 
	so we can track complete cycles in backtracing critical paths.  
	Also track unknowns for debugging bad circuits.  
	Status: not quite begun.
	Goal: 1
 */
#define	PRSIM_SEPARATE_CAUSE_NODE_DIRECTION		0

/**
	Define to 1 to enable configurable behavior of prsim
	on an unstable event.  
	original behavior: unstable events propagate X (conservative)
	new behavior: unstable events are just dequeued
	Mode commands: unstable-{unknown,dequeue}.
	Status: implemented and basically tested
	Goal: 1
 */
#define	PRSIM_ALLOW_UNSTABLE_DEQUEUE			1

/**
	TODO: fine-grain control of simulator behavior on anomalous events,
	instability, interference: break or notify?
	Suggestion: {unstable,interfere}-{notify,silent},{break,nobreak}.
	Status: not begun
	Goal: 1
 */
#define	PRSIM_FINE_GRAIN_ERROR_CONTROL		0

/**
	Define to 1 to test fix of false interference.  
	Status: in progress
	Goal: 1
 */
#define	PRSIM_FIX_BOGUS_INTERFERENCE		1

//=============================================================================

#endif	// __HAC_SIM_PRSIM_DEVEL_SWITCHES_H__

