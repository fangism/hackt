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
	$Id: devel_switches.h,v 1.3.2.4 2006/08/11 02:05:50 fang Exp $
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
	Consequence: more memory per node, due to increased structure size.  
	Status: in progress.
	Goal: 1
 */
#define	PRSIM_SEPARATE_CAUSE_NODE_DIRECTION		1

//-----------------------------------------------------------------------------
// ready-to-commit the flags below

/**
	Define to 1 to assign the cause of a node event when the 
	event is dequeued, as opposed to when it is enqueued.  
	Otherwise unstable events will be wrongly attributed.  
	Goal: 1
	Status: ready-to-commit
 */
#define	PRSIM_ASSIGN_CAUSE_ON_DEQUEUE		1

/**
	TODO: fine-grain control of simulator behavior on anomalous events,
	instability, interference: break or notify?
	Status: complete, ready to commit
	Goal: 1
 */
#define	PRSIM_FINE_GRAIN_ERROR_CONTROL		1

/**
	Define to 1 to include cause-rules in event queue checkpointing.
	Should omit because rule/expr indices are optimization-sensitive,
	but we want checkpoints to be optimization-agnostic.
	Fortunately, cause_rule is only used in delay calculations.  
	Relocate this comment once it is committed.  
	Goal: 0
 */
#define	PRSIM_CHECKPOINT_CAUSE_RULE		0

//=============================================================================

#endif	// __HAC_SIM_PRSIM_DEVEL_SWITCHES_H__

