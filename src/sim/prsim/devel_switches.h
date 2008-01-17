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
	$Id: devel_switches.h,v 1.4.74.1 2008/01/17 01:32:31 fang Exp $
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
	Status: done and tested, save for new dependent features.
	Goal: 1
	Status: completed, stable, tested for a long time, perm-ready
		Only reason to not use: memory reduction.
 */
#define	PRSIM_SEPARATE_CAUSE_NODE_DIRECTION		1

/**
	Whether or not cause history should bother tracking timestamps.  
	Might be a nice option to have as a compile-time switch.  
	Goal: ?
	Rationale: slack time and critical path analysis
	Priority: low
	Cost: increase in memory proportional to number of unique nodes.
 */
#define	PRSIM_TRACK_CAUSE_TIME				0

/**
	Define to 1 to support 'weak' flavored rules in prsim.
	Generally speaking, weak rules are overpowered by non-weak rules, 
	and can also drive rules that are otherwise not driven.  
	Goal: 1
	Rationale: explicit staticizers, memories with bidirectional bitlines
	Note: this should not cause any regressions on previous simulations
		that were never aware of the weak attribute.  
	Status: drafted, somewhat tested, needs more rigorous testing.
	Priority: TOP
 */
#define	PRSIM_WEAK_RULES				1

/**
	Define to 1 to allow later events to overtake ones already
	in the event/pending queue.  A unknown-pull may be overtaken
	by a strong-pull if it is in the same direction.  Likewise, 
	vacuous pulls may be overtaken.  
	Rationale: synchronous circuit simulation require accommodation
	for what are normaly considered violations and anomalies.  
	Goal: 1
	Status: drafted, minimally tested
	Note: this should be orthogonal to weak rules
	Rationale: for synchronous circuit simulation.  
	Priority: HIGH
 */
#define	PRSIM_ALLOW_OVERTAKE_EVENTS			1

//-----------------------------------------------------------------------------
// ready-to-commit the flags below

/**
	Define to 1 to include cause-rules in event queue checkpointing.
	Should omit because rule/expr indices are optimization-sensitive,
	but we want checkpoints to be optimization-agnostic.
	Fortunately, cause_rule is only used in delay calculations.  
	Relocate this comment once it is committed.  
	Goal: 0
	Status: been this way for a while, perm-ready
 */
#define	PRSIM_CHECKPOINT_CAUSE_RULE		0

//=============================================================================

#endif	// __HAC_SIM_PRSIM_DEVEL_SWITCHES_H__

