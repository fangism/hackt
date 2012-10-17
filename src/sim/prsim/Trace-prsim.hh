/**
	\file "sim/prsim/Trace-prsim.hh"
	$Id: Trace-prsim.hh,v 1.2 2009/02/01 07:21:45 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_TRACE_PRSIM_H__
#define	__HAC_SIM_PRSIM_TRACE_PRSIM_H__

#include "sim/trace_common.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
class State;
class NodeState;

/***
Challenges:
Handling channel-caused events, since they do not correspond to any rules.  
***/

//=============================================================================
/**
	Since the simulator only traces boolean nodes, we can unify
	the event structure used in the trace (unlike chpsim).  
	There is a 1-to-1 correspondence between events and value changes
	in prsim.  

	event_trace_point::event_id is interpreted as the static global rule
	(index) that fired to cause this transition.  
	Given a global rule index, we can deduce the index of the node whose
	value changed, and save memory.  
	Events which are not caused by rules, however, will default to 
	referencing 0, which is interpreted as an external event.  
	Such events can come from channel environments or user-induced events.  
 */
struct state_trace_point : public event_trace_point {
	typedef	char				value_type;
	using event_trace_point::timestamp;
	/**
		The global index of the node that switched.
		This can usually be inferred from the rule index.  
	 */
	node_index_type				node_index;
protected:
	/**
		The direction of the rule should suffice to deduce 
		the value of the node!
		X's will not be deducible.
		We use this byte field to also store the node's 
		*previous* value, which makes the trace file
		extremely efficient for rewinding (playing in reverse!)
		The only case where the old value will not be the 
		opposite of the new value is when there is an X.  
		The new value is stored in the 4 LSB, old value in 4 MSB.
	 */
	value_type				node_value;
public:
	state_trace_point() { }		// uninitialized
	/**
		\param nv node's new value
		\param ov node's old value
	 */
	state_trace_point(const time_type& t, const trace_index_type ei, 
		const trace_index_type c, const node_index_type ni, 
		const char nv, const char ov) : event_trace_point(t, ei, c), 
		node_index(ni), node_value(value_type(ov << 4) | nv) { }

	value_type
	new_value(void) const {
		return node_value & 0x0F;
	}

	value_type
	old_value(void) const {
		return node_value >> 4;
	}

// for memory alignment (performance), 
// character values are written out in a separate array
	void
	write_no_value(ostream&) const;

	void
	write_value_only(ostream&) const;

	void
	read_no_value(istream&);

	void
	read_value_only(istream&);

	ostream&
	__dump(ostream&) const;

	ostream&
	dump(ostream&) const;

};	// end class state_trace_point

//=============================================================================
/**
	One of these entries is added each time a node changes value.

	state_trace_point_base::global_index is interpreted as the 
	global node index.
	This need not be written to file if the corresponding
	static-event referenced by the (dynamic) event index is
	nonzero and corresponds to a global rule allocated in the 
	simulator state (and can be reconstructed).  
 */
class trace_chunk : public vector<state_trace_point> {
	typedef	vector<state_trace_point>	event_array_type;

public:

	size_t
	event_count(void) const { return size(); }

	const trace_time_type&
	start_time(void) const {
		INVARIANT(size());
		return front().timestamp;
	}

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&, const trace_index_type, const State&) const;

};	// end class state_trace_point

//=============================================================================
/**
	Trace file manager for prsim.
 */
class TraceManager : public trace_manager_base {
	typedef	vector<NodeState>			checkpoint_type;
	typedef	state_trace_point::value_type		node_value_type;
	typedef	vector<node_value_type>		node_value_array;
private:
	/**
		Current record of recent history.  
	 */
	trace_chunk				current_chunk;
	/**
		Current snapshot of the current state of all nodes.  
	 */
	const checkpoint_type&			checkpoint;
private:
	// for temporary construction only
	explicit
	TraceManager(const checkpoint_type&);
public:
	TraceManager(const string&, const checkpoint_type&);

	// warn if trace file is not finished!
	~TraceManager();

	/**
		\return index of the new traced event, starting at 0.
	 */
	trace_index_type
	push_back_event(const state_trace_point& p) {
		const size_t s = current_chunk.size();
		current_chunk.push_back(p);
		return s +previous_events;
	}

	trace_index_type
	last_event_trace_id(void) const {
		return current_chunk.event_count() +previous_events;
	}

	void
	flush(void);

	/// \return number of events accumulated in since last flush
	size_t
	current_event_count(void) const {
		return current_chunk.event_count();
	}

private:
	void
	__write_checkpoint(void);

	void
	__force_flush(void);

	ostream&
	dump_checkpoint(istream&, ostream&);

public:
	// text-dump?
	// load?

	static
	void
	text_dump(ifstream&, ostream&, const State&);
	 // we all stream for istream!

	static
	bool
	text_dump(const string&, ostream&, const State&);

#if 0
	// defined in "sim/prsim/TraceStreamer.h"
	class entry_streamer;
	class entry_reverse_streamer;
	class random_accessor;
	class state_change_streamer;
#endif
};	// end class TraceManager

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_TRACE_PRSIM_H__

