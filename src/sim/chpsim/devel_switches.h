/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.5.6.9 2007/04/29 05:56:32 fang Exp $
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
	Status: in development, paused until we take care of changing
		the execution model first.  
	Co-dependent: CHPSIM_DELAYED_SUCCESSOR_CHECKS
 */
#define	CHPSIM_COUPLED_CHANNELS			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to support up to two atomic events per step.  
	Places affected: event_placeholder_type, ...
	Rationale: perfect synchronization that doesn't split up the event
		pair in FIFO scheduling.
	Goal: 1
	Priority: high
	Status: not begun, will wait until 
	Prerequisite: CHPSIM_DELAYED_SUCCESSOR_CHECKS (?)
	May not use this if we accept atomic events being split
	in the immediate event fifo (pseudo atomic).
 */
#define CHPSIM_EVENT_PAIRS			0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to apply prefix delays *before* events are first checked.
	Rationale: to faciliate send/receive atomicity/simultaneity.
	Goal: 1
	Priority: TOP
	Status: drafted, in early testing
	Prerequisite: none
	Plan: instate a first_recheck_queue, where successors first arrive.
		step() will now recheck events in order until one (or two)
		*actually* executes.  
	Co-dependent: CHPSIM_COUPLED_CHANNELS
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
	Define to 1 to implement CHP event execution and rechecking
	as a visitor instead of a direct virtual method.  
	Cost: execution slower due to double virtual dispatch
	Benefit: acyclic library dependence
	Goal: 1?
	Status: done, stably tested
	Priority: high -- for shared library arrangements
	TODO: perm this after merge to mainline because we cannot
		support previous versions with CHPSIM_DELAYED_SUCCESSOR_CHECKS.
 */
#define	CHPSIM_VISIT_EXECUTE			1

//=============================================================================

#endif	// __HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

