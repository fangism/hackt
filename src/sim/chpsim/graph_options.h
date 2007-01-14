/**
	\file "sim/chpsim/State.h"
	$Id: graph_options.h,v 1.1.2.1 2007/01/14 03:00:23 fang Exp $
	Structure that contains the state information of chpsim.  
 */

#ifndef	__HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__
#define	__HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
/**
	Finer control over dumping options.
	TODO: options to draw edges to instances
 */
struct graph_options {
	bool				show_event_index;

	graph_options();
};	// end struct state_dump_options

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_GRAPH_OPTIONS_H__

