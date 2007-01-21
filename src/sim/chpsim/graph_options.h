/**
	\file "sim/chpsim/graph_options.h"
	$Id: graph_options.h,v 1.2 2007/01/21 06:00:48 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__
#define	__HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__

#include "sim/chpsim/devel_switches.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
/**
	Finer control over dumping options.
	NOTE: definition for this class resides in
		"sim/chp
 */
struct graph_options {
	/**
		When true, prints the event ID number with the node label.
		Default: true
	 */
	bool				show_event_index;
	/**
		Show physical instances as nodes (bool, int, enum, channel)
		and also draw edges to and from them to show
		dependencies and anti-dependencies.
		Implies with_antidependencies.
		Default: false
	 */
	bool				show_instances;
#if CHPSIM_READ_WRITE_DEPENDENCIES
	/**
		Evaluate lvalue antidependencies (may sets) for
		all CHP events.  Dependencies are always evaluated, 
		but anti-dependencies are only needed for graph visualization, 
		never for run-time simulation.  
		Default: false
		Implied by show_instances.
	 */
	bool				with_antidependencies;
#endif
	/**
		If true, draws event subgraphs wrapped in clusters
		by process index.  
		Default: false
	 */
	bool				process_event_clusters;


	graph_options();
};	// end struct state_dump_options

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__

