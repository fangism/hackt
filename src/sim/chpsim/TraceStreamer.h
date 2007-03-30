/**
	\file "sim/chpsim/TraceStreamer.h"
	$Id: TraceStreamer.h,v 1.1.2.4 2007/03/30 15:47:58 fang Exp $
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

//=============================================================================
/**
	Helper struct for incrementally reading
	trace entries in sequential order.  
	We only need to keep track of position-state.  
	The scheme interface uses this class.  
	This class may be heap-allocated and managed by guile's gc.
 */
class TraceManager::entry_streamer {
protected:
	ifstream					fin;
	TraceManager					tracefile;
	trace_file_contents::const_iterator		epoch_iter;
	event_trace_window::const_iterator		event_iter;
	size_t						_index;
protected:
	struct partial_init_tag { };

	entry_streamer(const string&, partial_init_tag);
public:
	explicit
	entry_streamer(const string&);

	~entry_streamer();

	const event_trace_point&
	current_event_record(void) const;

	bool
	good(void) const;

	size_t
	index(void) const { return _index; }

protected:
	good_bool
	partial_init(void);

private:
	good_bool
	init(void);

public:
	good_bool
	advance(void);

};	// end struct TraceManager::entry_streamer

//-----------------------------------------------------------------------------
/**
	Streams the trace entries in reverse order.
	Replaces advance() with retreat().
 */
class TraceManager::entry_reverse_streamer : 
		protected entry_streamer {
	typedef	entry_streamer		parent_type;
	/// offset to be able to seek epochs
	size_t				start_of_epochs;
public:
	explicit
	entry_reverse_streamer(const string&);

	const event_trace_point&
	current_event_record(void) const;

	bool
	good(void) const;

	using parent_type::index;

private:
	good_bool
	init(void);

public:
	good_bool
	retreat(void);

};	// end class trace_entry_reverse_streamer

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACESTREAMER_H__

