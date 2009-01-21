/**
	\file "sim/chpsim/Trace.h"
	$Id: Trace.h,v 1.5.28.2 2009/01/21 00:04:56 fang Exp $
	Simulation execution trace structures.  
	To reconstruct a full trace with details, the object file used
	to simulate must be loaded.  
 */

#ifndef	__HAC_SIM_CHPSIM_TRACE_H__
#define	__HAC_SIM_CHPSIM_TRACE_H__

#include "sim/trace_common.h"
#include "Object/nonmeta_variable.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class State;		// be-friend me
class TraceManager;
using entity::variable_type;
using entity::state_data_extractor;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;

//=============================================================================
/**
	One of these objects is appended anytime a variable's state is
	touched (even if the result doesn't change its value).  
	Meta-type specific trace point.  
	Aligned or packed?
 */
template <class Tag>
struct state_trace_point : public state_trace_point_base {
	typedef	state_data_extractor<Tag>	extractor_policy;
	typedef	typename extractor_policy::var_type	var_type;
	typedef	typename extractor_policy::value_type	value_type;
	value_type				raw_data;

	state_trace_point() { }		// uninitialized

	state_trace_point(const value_type&, 
		const trace_index_type, const size_t);

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end struct state_trace_point

//=============================================================================
/**
	Time-stamped value history.  
	An alternative is to embed history into the state itself, 
	which is a more intrusive approach.  
 */
template <class Tag>
class state_trace_window_base {
protected:
	typedef	state_data_extractor<Tag>	extractor_policy;
	typedef	typename extractor_policy::var_type	var_type;
	typedef	typename extractor_policy::value_type	value_type;
	typedef	state_trace_point<Tag>		data_type;
	typedef	std::vector<data_type>		data_array_type;
	/**
		Iterator that only conditionally increments.  
		Useful for emulating a stream of state changes.  
	 */
	struct __pseudo_const_iterator;
	struct __pseudo_const_iterator_range;
public:
	typedef	typename data_array_type::const_iterator	iter_type;
	typedef	std::pair<iter_type, iter_type>	__pseudo_const_iterator_pair;

protected:
	data_array_type				data_array;

//	state_trace_window_base();
//	~state_trace_window_base();

	void
	__push_back(const var_type&, const trace_index_type, 
			const size_t);

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end struct trace_window_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	History of events.  
	Slice of the trace in time, capturing all data and execution
	events in a range of time.  
	Constructed from reading trace-file by chunks.  
	This is also the structure that is periodically flushed out
	during simulation.  
	This is the chunk that is the unit of writing, as summarized
	by the trace's table of contents.  
	TODO: use reserved vectors to avoid expensive reallocation, 
		if same trace window is reused, then reserve will persist
		for subsequent time windows.  So don't delete this yet!
 */
class state_trace_time_window : 
		protected state_trace_window_base<bool_tag>,
		protected state_trace_window_base<int_tag>,
		protected state_trace_window_base<enum_tag>,
		protected state_trace_window_base<channel_tag> {
protected:
	typedef state_trace_window_base<bool_tag>::__pseudo_const_iterator
				bool_pseudo_const_iterator;
	typedef state_trace_window_base<int_tag>::__pseudo_const_iterator
				int_pseudo_const_iterator;
	typedef state_trace_window_base<enum_tag>::__pseudo_const_iterator
				enum_pseudo_const_iterator;
	typedef state_trace_window_base<channel_tag>::__pseudo_const_iterator
				channel_pseudo_const_iterator;

	typedef state_trace_window_base<bool_tag>::__pseudo_const_iterator_range
				bool_pseudo_const_iterator_range;
	typedef state_trace_window_base<int_tag>::__pseudo_const_iterator_range
				int_pseudo_const_iterator_range;
	typedef state_trace_window_base<enum_tag>::__pseudo_const_iterator_range
				enum_pseudo_const_iterator_range;
	typedef state_trace_window_base<channel_tag>::__pseudo_const_iterator_range
				channel_pseudo_const_iterator_range;
public:
	struct pseudo_const_iterator;
	struct pseudo_const_iterator_range;
public:
	/**
		Template forwarding function.  
	 */
	template <class Tag>
	void
	push_back(const typename variable_type<Tag>::type& v, 
		const trace_index_type t, const size_t g) {
		state_trace_window_base<Tag>::__push_back(v, t, g);
	}

protected:
	template <class Tag>
	typename state_trace_window_base<Tag>::pseudo_const_iterator
	end(void) const {
		return state_trace_window_base<Tag>::data_array.end();
	}

public:
	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end class trace_time_window

//=============================================================================
/**
	Since we can't afford to hold the entire trace memory, we use 
	store buffer of trace data to be flushed out.  
	Accumulate some data, then flush it out periodically.  
	TODO: how to load or reconstruct traces by window.
	TODO: trace file with table of contents, and table of file offsets
		of where to jump.  
 */
struct trace_chunk : 
		public state_trace_time_window,
		public event_trace_window {
	trace_chunk();
	~trace_chunk();

	using event_trace_window::push_back_event;
	using event_trace_window::end;

	size_t
	event_count(void) const {
		return event_array.size();
	}

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&, const size_t pe) const;

};	// end struct trace_chunk

//=============================================================================
// TODO: trace slice extraction methods -- when only a piece is required

//=============================================================================
/**
	Minimal trace information to reproduce all information.
	Trace should be replayable.
	TODO: decide what to record, what to reconstruct.  
	TODO: trace consistency and integrity checks.  
	TODO: record chpsim invoked options and compare on trace load
 */
class TraceManager : public trace_manager_base {
private:
	/**
		Current record of recent history.  
	 */
	trace_chunk				current_chunk;
private:
	// for temporary construction only
	TraceManager();
public:
	explicit
	TraceManager(const string&);

	// warn if trace file is not finished!
	~TraceManager();

	trace_index_type
	push_back_event(const event_trace_point& p) {
		return current_chunk.push_back_event(p) +previous_events;
	}

	template <class Tag>
	void
	push_back_data(const typename variable_type<Tag>::type& v, 
			const trace_index_type t, const size_t g) {
		current_chunk.push_back<Tag>(v, t, g);
	}

	trace_index_type
	last_event_trace_id(void) const {
		return current_chunk.event_count() +previous_events;
	}

	void
	flush(void);

#if 0
	size_t
	get_previous_events(void) const {
		return previous_events;
	}
#endif

	/// \return number of events accumulated in since last flush
	size_t
	current_event_count(void) const {
		return current_chunk.event_count();
	}

public:
	// text-dump?
	// load?

	static
	void
	text_dump(ifstream&, ostream&);	// we all stream for istream!

	static
	bool
	text_dump(const string&, ostream&);

	// defined in "sim/chpsim/TraceStreamer.h"
	class entry_streamer;
	class entry_reverse_streamer;
	class random_accessor;
	class state_change_streamer;

};	// end class TraceManager

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACE_H__

