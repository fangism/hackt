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
	$Id: devel_switches.h,v 1.11 2008/11/24 20:49:53 fang Exp $
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
	Status: Perm'd
		Only reason to not use: memory reduction.
 */
// #define	PRSIM_SEPARATE_CAUSE_NODE_DIRECTION		1

/**
	Whether or not cause history should bother tracking timestamps.  
	Might be a nice option to have as a compile-time switch.  
	Goal: ?
	Rationale: slack time and critical path analysis
	Priority: low
	Cost: increase in memory proportional to number of unique nodes.
	Alternative: rely on history tracing (trace files), perhaps
		using some finite window for history keeping.  
 */
#define	PRSIM_TRACK_CAUSE_TIME				0

/**
	Define to 1 to enable tracing, recording event history.  
	Application: post-mortem analysis, performance analysis, 
	bug root-causing, possible rewinding (time machine!).
	Goal: 1
 */
#define	PRSIM_TRACE_GENERATION				0

/**
	Define to 1 to support 'weak' flavored rules in prsim.
	Generally speaking, weak rules are overpowered by non-weak rules, 
	and can also drive rules that are otherwise not driven.  
	Goal: 1 (for now)
	Rationale: explicit staticizers, memories with bidirectional bitlines
	Note: this should not cause any regressions on previous simulations
		that were never aware of the weak attribute.  
	Status: done, fairly tested
	This will eventually be superceded by PRSIM_NEW_EVENT_MODEL, below.
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
	Status: done, fairly tested
	Note: this should be orthogonal to weak rules
	Rationale: for synchronous circuit simulation.  
 */
#define	PRSIM_ALLOW_OVERTAKE_EVENTS			1

/**
	Define to 1 to allow new generalized error-handling policies
	for various error conditions.
	Rationale: finer-grain user control of error handling, 
		and granting user opportunity to interactively inspect.
	Goal: 1
	Status: begun
 */
#define	PRSIM_NEW_ERROR_POLICIES			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	INVASIVE change.
	Eliminate pre-translated, allocated top-level expressions in favor
	of performing footprint translationsat run-time.
	This also significantly changes the way node fanin will work
	because of process hierarchy; cannot use existing OR-combinations.  
	Tradeoff: slower, but much more memory-scalable.
	Rationale: memory is more critical for massive designs
	Priority: HIGH
	Goal: 1
	Status: complete, debugged, merged to trunk
	When this is perm'd refactor out classes and methods from
		State-prsim.cc.
 */
#define	PRSIM_INDIRECT_EXPRESSION_MAP			1

/**
	Define to 1 to create special pseudo-rules which have no real
	output node, but are actually run-time invariant checks.  
	Will require a new rule flag in the bit-field.  
	Rationale: stronger assertion checking
	Status: done, tested, can perm
	Goal: 1
 */
#define	PRSIM_INVARIANT_RULES		(1 && PRSIM_INDIRECT_EXPRESSION_MAP)

/**
	Define to 1 to move direction flag to rules, currently in expressions.
	Rationale: because it makes sense.
	Status: done, tested, can perm
	Goal: 1
 */
#define	PRSIM_RULE_DIRECTION		(1 && PRSIM_INDIRECT_EXPRESSION_MAP)

/**
	Eventually unify the structures and enumerations between
	sim/prsim/Expr.h and Object/lang/PRS_footprint_expr.h.
	This will save on per-type memory footprint.  
	Tradeoff: will result in slightly larger memory footprint
	for process PRS footprints, but eliminate redundant structures.
	Issue: should there be graph optimization, like prsim -O1?
	Goal: 1?
	Prerequisite: PRSIM_INDIRECT_EXPRESSION_MAP
	Rationale: doing this in a separate second rewrite phase 
	to minimize the invasiveness of change, 
	for maintainability and stability.  
 */
#define	PRSIM_UNIFY_GRAPH_STRUCTURES	(0 && PRSIM_INDIRECT_EXPRESSION_MAP)

/**
	Summary: Also plan to use a hierarchical fanin/fanout structure, 
	where lookups will ascend to parents through ports, 
	and descend through children through ports.  
	Rationale: This will eliminate fanin-fanout caching, memory-hog.  
	May require static direction-tracking of nodes!
	Difficulty: supprorting random eval-ordering, or any shuffling...
	Goal: ?
 */
#define	PRSIM_HIERARCHICAL_FANOUT_ONLY	(0 && PRSIM_INDIRECT_EXPRESSION_MAP)

/**
	In addition to un-caching expression structures, also maintain
	check and force-excl rings per process type.  
	Might be more difficult, less benefit?
	Goal: ?
	Rationale: further memory reduction
 */
#define	PRSIM_HIERARCHICAL_RINGS	(0 && PRSIM_INDIRECT_EXPRESSION_MAP)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Complete rewrite of prsim event system core, accounting properly
	for strength of rules, etc.
	Rationale: the current hacked spaghetti mess is both
		incomprehensible and unmaintainable.
		Rewrite will gain back performance, from simplicity.
	Status: not begun
	Priority: medium
	Goal: 1
 */
#define	PRSIM_NEW_EVENT_MODEL				0

// TODO: prsim-guile interface

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

