/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.2.2.5 2007/02/03 05:30:55 fang Exp $
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
	Status: not begun
 */
#define	CHPSIM_CHECKPOINTING			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to track last-event to cause.
	This is done at enqueue-time.  
	Rationale: critical path and efficient backward causality tracing.  
	Goal: 1
	Priority: medium
	Status: done and basically tested
 */
#define	CHPSIM_CAUSE_TRACKING			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define to 1 to enable tracing capabilities.
	Goal: 1
	Rationale: thesis work -- trace-mining
	Priority: high (TOP)
	Prerequisite: possibly CHPSIM_CAUSE_TRACKING
	Status: first version done, basics tested
	Q: what do we do about random timing?
 */
#define	CHPSIM_TRACING				1

//=============================================================================

#endif	// __HAC_SIM_CHPSIM_DEVEL_SWITCHES_H__

