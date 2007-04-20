/**
	\file "sim/chpsim/TraceStreamer.h"
	$Id: TraceStreamer.h,v 1.2 2007/04/20 18:26:14 fang Exp $
	Simulation execution trace structures.  
	To reconstruct a full trace with details, the object file used
	to simulate must be loaded.  
 */

#ifndef	__HAC_SIM_CHPSIM_TRACESTREAMER_H__
#define	__HAC_SIM_CHPSIM_TRACESTREAMER_H__

#include "sim/chpsim/TraceIterators.h"
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
	TODO: factor this into a protected base class.
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
protected:
	/// offset to be able to seek epochs
	size_t				start_of_epochs;
	/// total number of entries in trace
	size_t				_total_entries;
public:
	explicit
	entry_reverse_streamer(const string&);

	const event_trace_point&
	current_event_record(void) const;

	bool
	good(void) const;

	using parent_type::index;

	size_t
	num_entries(void) const { return _total_entries; }

private:
	good_bool
	init(void);

public:
	good_bool
	retreat(void);

};	// end class TraceManager::entry_reverse_streamer

//=============================================================================
/**
	Interface for accessing random entries in trace file.  
	Why inherit? we just want the start_of_epochs.
	Consecutive access that 'hit' the same epoch will be efficient.
	Each time an epoch is re-loaded, we incur file reading cost.  
	This class keeps one epoch loaded at a time.  
 */
class TraceManager::random_accessor : protected entry_reverse_streamer {
	typedef	entry_reverse_streamer			parent_type;
protected:
public:
	explicit
	random_accessor(const string&);

	// index must be valid, pre-checked
	const event_trace_point&
	operator [] (const size_t);

	using parent_type::num_entries;

	bool
	good(void) const;

};	// end class TraceManager::random_accessor

//=============================================================================
/**
	This forward stream (slice) indicates what values and state were 
	changed with each event.  
	The parent's entry_iter is never used to get entry reference.  
 */
class TraceManager::state_change_streamer : protected entry_streamer {
	typedef	entry_streamer				parent_type;
public:
	typedef	state_trace_time_window::pseudo_const_iterator_range
					pseudo_const_iterator_range;
protected:
	pseudo_const_iterator_range			state_iter;
public:
	explicit
	state_change_streamer(const string&);

	// default parent destructor

	using parent_type::index;
	using parent_type::good;

	good_bool
	advance(void);

	const pseudo_const_iterator_range&
	current_state_iter(void) const {
		return state_iter;
	}
private:
	void
	begin(void);
};	// end class TraceManager::entry_streamer

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACESTREAMER_H__

