/**
	\file "sim/chpsim/TraceStreamer.h"
	$Id: TraceStreamer.h,v 1.1.2.1 2007/03/20 23:10:45 fang Exp $
	Simulation execution trace structures.  
	To reconstruct a full trace with details, the object file used
	to simulate must be loaded.  
 */

#ifndef	__HAC_SIM_CHPSIM_TRACESTREAMER_H__
#define	__HAC_SIM_CHPSIM_TRACESTREAMER_H__

#include "sim/chpsim/Trace.h"
#include <fstream>
#include "util/boolean_types.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using util::good_bool;

/**
	Helper struct for incrementally reading
	trace entries in sequential order.  
	We only need to keep track of position-state.  
	The scheme interface uses this class.  
	This class may be heap-allocated and managed by guile's gc.
 */
class TraceManager::entry_streamer {
	ifstream					fin;
	TraceManager					tracefile;
	trace_file_contents::const_iterator		epoch_iter;
	event_trace_window::const_iterator		event_iter;
public:
	explicit
	entry_streamer(const string&);

	~entry_streamer();

	const event_trace_point&
	current_event_record(void) const;

	bool
	good(void) const;

private:
	good_bool
	init(void);

public:
	good_bool
	advance(void);

};	// end struct TraceManager::entry_streamer

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACESTREAMER_H__

