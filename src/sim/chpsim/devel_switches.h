/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.7.2.2 2007/06/11 20:22:11 fang Exp $
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
	Define to 1 to implement channels using tight coupling, 
		i.e. truly blocking send-receive atomic pairs.  
	Rationale: channels should have slack 0
		without this, channels inherently have slack 1 because
		the occupancy bit does not block a channel.  
	Goal: 1
	Priority: top
	Status: done, tested, ready to perm.
	Co-dependent: CHPSIM_DELAYED_SUCCESSOR_CHECKS
 */
#define	CHPSIM_COUPLED_CHANNELS			1

/**
	Define to 1 to apply prefix delays *before* events are first checked.
	Rationale: to faciliate send/receive atomicity/simultaneity.
	Goal: 1
	Priority: TOP
	Status: done, tested, ready to perm.
	Co-dependent: CHPSIM_COUPLED_CHANNELS
	Plan: instate a first_recheck_queue, where successors first arrive.
		step() will now recheck events in order until one (or two)
		*actually* executes.  
 */
#define	CHPSIM_DELAYED_SUCCESSOR_CHECKS		1

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
	Define to 1 to construct one canonical process event subgraph per
	complete type (with CHP) to make allocation easier, and faster.
	Also makes events per process contiguous in allocation, making
	reverse-mapping much easier.  
	Goal: 1
	Status: not begun
	Priority: low-medium
 */
#define	CHPSIM_FOOTPRINTIZE_EVENTS		0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to print hierarchical alias instead of definition-local
	names in chp dumps.  
	Goal: 1
	Rationale: feedback readability
	Priority: medium
	Status: done, tested, ready to perm.
 */
#define	CHPSIM_DUMP_PARENT_CONTEXT		1

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to support nonmeta-reference supplementing using
		statically prebound instance indices.  
	Rationale: [optimization] elide looking up statically resolved 
		references, a suspected performance bottleneck.
	Strategy: ?
	Goal: 1
	Status: not begun
	Priority: low-medium
 */
#define	CHPSIM_CACHE_META_REFERENCES		0

//=============================================================================

#endif	// __HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

