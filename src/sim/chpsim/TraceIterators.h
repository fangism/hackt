/**
	\file "sim/chpsim/TraceIterators.h"
	$Id: TraceIterators.h,v 1.2 2007/04/20 18:26:13 fang Exp $
	Nested iterator class definitions
 */

#ifndef	__HAC_SIM_CHPSIM_TRACEITERATORS_H__
#define	__HAC_SIM_CHPSIM_TRACEITERATORS_H__

#include "sim/chpsim/Trace.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {

//=============================================================================
/**
	Time-stamped value history.  
	An alternative is to embed history into the state itself, 
	which is a more intrusive approach.  
 */
template <class Tag>
class state_trace_window_base<Tag>::__pseudo_const_iterator {
	iter_type			__iter;
protected:
	/// invalid initialize 
	__pseudo_const_iterator() : __iter() { }

	void
	begin(const state_trace_window_base& w) {
		this->__iter = w.data_array.begin();
	}
public:
	// allow implicit conversion
	__pseudo_const_iterator(const iter_type i) : __iter(i) { }

	explicit
	__pseudo_const_iterator(const state_trace_window_base& a) :
		__iter(a.data_array.begin()) { }

};	// end struct __pseudo_const_iterator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
class state_trace_window_base<Tag>::__pseudo_const_iterator_range {
	__pseudo_const_iterator_pair		p;

	void
	begin(const state_trace_window_base& w) {
		p.second = p.first = w.data_array.begin();
	}

	void
	advance(const trace_index_type, const state_trace_window_base&);

};	// end struct __pseudo_const_iterator_range

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
struct state_trace_time_window::pseudo_const_iterator : 
		public bool_pseudo_const_iterator,
		public int_pseudo_const_iterator,
		public enum_pseudo_const_iterator,
		public channel_pseudo_const_iterator {
	
	pseudo_const_iterator() : 
		bool_pseudo_const_iterator(), 
		int_pseudo_const_iterator(), 
		enum_pseudo_const_iterator(), 
		channel_pseudo_const_iterator() { }

	explicit
	pseudo_const_iterator(const state_trace_time_window& w) :
		bool_pseudo_const_iterator(w), 
		int_pseudo_const_iterator(w), 
		enum_pseudo_const_iterator(w), 
		channel_pseudo_const_iterator(w) { }

	pseudo_const_iterator(
		const bool_pseudo_const_iterator b, 
		const int_pseudo_const_iterator i, 
		const enum_pseudo_const_iterator e, 
		const channel_pseudo_const_iterator c) :
		bool_pseudo_const_iterator(b), 
		int_pseudo_const_iterator(i), 
		enum_pseudo_const_iterator(e), 
		channel_pseudo_const_iterator(c) { }

};	// end class pseudo_const_iterator

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct state_trace_time_window::pseudo_const_iterator_range :
		public bool_pseudo_const_iterator_range,
		public int_pseudo_const_iterator_range,
		public enum_pseudo_const_iterator_range,
		public channel_pseudo_const_iterator_range {

	void
	begin(const state_trace_time_window& w) {
		bool_pseudo_const_iterator_range::begin(w);
		int_pseudo_const_iterator_range::begin(w);
		enum_pseudo_const_iterator_range::begin(w);
		channel_pseudo_const_iterator_range::begin(w);
	}


	void
	advance(const trace_index_type, const state_trace_time_window&);

	template <class Tag>
	const typename state_trace_window_base<Tag>::__pseudo_const_iterator_pair&
	get(void) const {
		return state_trace_window_base<Tag>::__pseudo_const_iterator_range::p;
	}

};	// end class pseudo_const_iterator_range

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACEITERATORS_H__

