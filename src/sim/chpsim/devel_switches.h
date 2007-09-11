/**
	\file "sim/chpsim/devel_switches.h"
	Development feature switches.  
	$Id: devel_switches.h,v 1.9 2007/09/11 06:53:14 fang Exp $
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
// Perm'd (00-01-04-main-00-81-68-chpsim-09-01)
// Summary: send-receive action pairs slack-zero
// #define	CHPSIM_COUPLED_CHANNELS			1

// Perm'd (00-01-04-main-00-81-68-chpsim-09-01)
// Summary: apply prefix-delay, i.e. before event is *first* checked
// #define	CHPSIM_DELAYED_SUCCESSOR_CHECKS		1

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
	Define to 1 to allocate events using the local event pools
	established by process footprints.  
	Rationale: speeds up allocation, reduces memory, and enables
		reverse-lookup of event to CHP language source.  
	Completed and perm'd on branch 00-01-04-main-00-81-68-chpsim-09.
#define	CHPSIM_BULK_ALLOCATE_GLOBAL_EVENTS		1
**/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Perm'd (00-01-04-main-00-81-68-chpsim-09-10)
// #define	CHPSIM_DUMP_PARENT_CONTEXT		1

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

