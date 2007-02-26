/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.3.2.2 2007/02/26 01:34:18 fang Exp $
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
	Priority: medium (enhancement to enable diagnostics)
		This is also necessary for a decent implementation of
		watch-points and break-points for linear time checking.  
	Status: done, and tested.
 */
#define	CHPSIM_STATE_UPDATE_BIN_SETS		1

/**
	Define to 1 to enable instance value watch points.  
	Note: watching individual events does not require this.  
	Prerequisite: CHPSIM_STATE_UPDATE_BIN_SETS, for sorting
	Goal: 1
	Priority: low-medium (for diagnostics)
	Status: beginning
 */
#define	CHPSIM_WATCH_VALUES		(0 && CHPSIM_STATE_UPDATE_BIN_SETS)

/**
	Define to 1 to enable instance value break points.  
	Note: breaking on individual events does not require this.  
	Goal: 1
	Priority: low-medium (for diagnostics)
	Status: beginning
 */
#define CHPSIM_BREAK_VALUES		(0 && CHPSIM_STATE_UPDATE_BIN_SETS)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checkpointing should provide assistance to playback in trace analyses.  
	Should be an option to the tracing framework.
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Q: what do we do about tracing with random timing?
	At least issue a warning that analysis will be nonsense?
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to embed checkpoints periodically in the saved trace
	for the sake of being able to quickly replay from the middle
	of a trace.  
	Goal: 1?
	Status: not begun
	Priority: ?
 */
#define	CHPSIM_TRACE_WITH_CHECKPOINT		0

//=============================================================================

#endif	// __HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

