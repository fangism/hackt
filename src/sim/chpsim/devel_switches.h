/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.2.2.9 2007/02/05 05:02:48 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__
#define	__HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

//=============================================================================
// leave this error enabled for released code
#if 0
#error	Production code should NOT include this header file.  \
	However, if you are developing, use this file as you see fit.  
#endif

//=============================================================================
/**
	Define to 1 to use std::multiset implementation of event queue
		instead of a heap-based priority_queue.
	Rationale: the sorting in the priority queue is not stable for
		ranges of equal priority elements.  
	Goal: 1
	Priority: medium-high for checkpoint consistency verification.  
	Status: complete, tested, test cases updated accordingly.  
		Can perm this, after it is better documented. 
	Nice result is that cause_event_id in traces is now guaranteed
		to be monotonic, since the queue is now FCFS w.r.t.
		equal time-stamped events.  
 */
#define	CHPSIM_MULTISET_EVENT_QUEUE		1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to always evaluate read-dependencies 
	and anti-dependencies upon graph construction.  
	Anti-dependencies are only ever needed for dot graph construction, 
	but never for simulation execution.  
	Pretty much only data reading/writing events need these, 
		send, receive, assign.  
	Consequence: memory bloat of events.  
		can be alleviated by pointer-indirection.  
	Goal: 1
	Priority: low-medium
	Pre-requisites: wait until ReadDependenceCollector, 
		and WriteDependenceCollector are written.
		Then may be the same, but certainly not the same
		as BlockDependenceCollector (should rename).  
 */
#define	CHPSIM_READ_WRITE_DEPENDENCIES		0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to re-implement State::__updated_list as a 
	bin of sets or reference indices.  
	Rationale: This solves the problem of uniqueness of update reference, 
		and is better restructuring for performance.
	Goal: 1
	Priority: low (enhancement)
 */
#define	CHPSIM_STATE_UPDATE_BIN_SETS		0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to enable checkpointing functions.  
	Rationale: not only to save and restore state for long simulations, 
		but as an assistance to playback in trace analyses.  
		Should be an option to the tracing framework.
	Goal: 1
	Priority: medium
	Status: done, in testing
 */
#define	CHPSIM_CHECKPOINTING			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Q: what do we do about tracing with random timing?
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to embed checkpoints periodically in the saved trace
	for the sake of being able to quickly replay from the middle
	of a trace.  
	Goal: 1?
	Status: not begun
	Prerequisites: CHPSIM_TRACING, CHPSIM_CHECKPOINTING (of course)
	Priority: ?
 */
#define	CHPSIM_TRACE_WITH_CHECKPOINT	(0 && CHPSIM_CHECKPOINTING)

//=============================================================================

#endif	// __HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

