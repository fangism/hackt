/**
	\file "sim/chpsim/Trace.h"
	$Id: Trace.h,v 1.1.2.9 2007/02/05 05:02:48 fang Exp $
	Simulation execution trace structures.  
	To reconstruct a full trace with details, the object file used
	to simulate must be loaded.  
 */

#ifndef	__HAC_SIM_CHPSIM_TRACE_H__
#define	__HAC_SIM_CHPSIM_TRACE_H__

#include "sim/time.h"
#include "sim/common.h"
#include "sim/chpsim/devel_switches.h"
#include "util/attributes.h"
#include <iosfwd>
#include <vector>
#include <string>
#include "Object/nonmeta_variable.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class State;		// be-friend me
class TraceManager;
using std::istream;
using std::ostream;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using entity::variable_type;
using entity::state_data_extractor;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using util::memory::excl_ptr;

/**
	NOTE: keep consistent with EventNode::time_type
 */
typedef	real_time			trace_time_type;
/**
	Might have to use 64b for large traces...
 */
typedef	event_index_type		trace_index_type;

//=============================================================================
/**
	Single point of data in the execution trace.  
	NOTE: this structure should be aligned for performance.  
	Do we track data-state modifications separately or at all?

	NOTE: given event's Read/Write dependence sets, and timestamp
	can narrow down set of variables that caused it to fire, 
	by searching candidate dependencies around the timestamp.  
 */
struct event_trace_point {
	/**
		The timestamp of this event, should be monotonically
		increasing through the trace.  
	 */
	typedef	trace_time_type		time_type;
	/**
		Time stamp of event.  
	 */
	time_type			timestamp;
	/**
		The index of the event that occured at this time.  
		NOTE: this might be a limiting factor when it comes
		to trace size, if this is only 32b.
		Consider making a 64b version.  
	 */
	trace_index_type		event_id;
	/**
		The index of the trace-event that caused this event to fire.  
		(Not the allocated index of the event.)
		This allows quick and instant construction of the 
		critical path.  Well, except when there are multiple
		last events that arrive at the same time (slack 0).  
		We should probably interpret 0 as a special NULL-value, 
			meaning "not-applicable" or "unknown".  
	 */
	trace_index_type			cause_id;

	event_trace_point() { }		// default uninitialized, lazy
	event_trace_point(const time_type& t, const trace_index_type ei, 
			const trace_index_type c = 0) :
			timestamp(t), event_id(ei), cause_id(c)
			{ }

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end struct event_trace_point

//=============================================================================
/**
	Window of event trace events.  
 */
class event_trace_window {
protected:
	typedef	std::vector<event_trace_point>	event_array_type;
	event_array_type		event_array;
	// default ctor and dtor

	/**
		\return the index of the new trace point.  
	 */
	trace_index_type
	push_back_event(const event_trace_point&);

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&, const size_t offset = 0) const;

public:
	size_t
	event_count(void) const {
		return event_array.size();
	}

	trace_time_type
	start_time(void) const;

	trace_time_type
	end_time(void) const;

};	// end class event_trace_window

//=============================================================================
/**
	Per data instance, a sparse list of times at which the data
	changed by some event.  
	This type needs to depend on the data/channel meta-type.  
	Track causality?
 */
struct state_trace_point_base {
	/**
		Trace index of the event that caused this change, 
		which tells the event ID.  
		From the event ID number, the time and cause can be deduced.  
	 */
	trace_index_type		event_index;
	/**
		The index referencing the allocated data.  
	 */
	size_t				global_index;

	state_trace_point_base() { }	// uninitialized

	state_trace_point_base(const trace_index_type e, const size_t g) :
		event_index(e), global_index(g) { }

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end struct state_trace_point_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		public state_trace_window_base<bool_tag>,
		public state_trace_window_base<int_tag>,
		public state_trace_window_base<enum_tag>,
		public state_trace_window_base<channel_tag> {
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
/**
	Header used to track all the chunks that have been written out.  
	This is written out last, and concatenated in front of
	the body containing all the chunks.  
 */
class trace_file_contents {
public:
	/**
		Entries are stored in an array.  
	 */
	struct entry {
		/**
			Start time of chunk.  
		 */
		trace_time_type			start_time;
		/**
			File offset where chunk begins.  
			This offset can be relative to the start
			of the body, which can then be corrected
			using the size of the header/table-of-contents.  
			NOTE: could use std::streampos...
		 */
		size_t				file_offset;
		/**
			Size of chunk.  
			NOTE: could use std::streampos...
		 */
		size_t				chunk_size;

		entry() { }	// undefined values
		entry(const trace_time_type t, const size_t o, const size_t s) :
			start_time(t), file_offset(o), chunk_size(s) { }

		// human readable
		ostream&
		dump(ostream&) const;

		void
		write(ostream&) const;

		void
		read(istream&);
	};	// end struct entry
private:
	typedef	vector<entry>			entry_array_type;
	/**
		The last chunk at the final finish time will be a 
		filler entry with offset and EOF and chunk_size 0.  
	 */
	entry_array_type			entry_array;
public:
	typedef	entry_array_type::const_iterator	const_iterator;
public:
	trace_file_contents();
	~trace_file_contents();

	void
	push_back(const entry& e) {
		entry_array.push_back(e);
	}

	const_iterator
	begin(void) const { return entry_array.begin(); }

	const_iterator
	end(void) const { return entry_array.end(); }

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	dump(ostream&) const;

};	// end class trace_file_contents


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
class TraceManager {
	friend class State;
private:
	/**
		Name of the trace file to output to.  
	 */
	string					trace_file_name;
	/**
		The name of a temporary file for staging
		trace concatenation.  
	 */
	string					temp_file_name;
	/**
		This is the stream to the temp file.  
		Bidirectional because we need to read it to finalize
		the output stream.  
	 */
	excl_ptr<fstream>			trace_ostream;
	/**
		This is the stream to the header, also the final file.  
		Nothing is actually written to this file until
		finish() is called.  
	 */
	excl_ptr<ofstream>			header_ostream;
	/**
		Table of contents.  
		This is the first section written to the
		
	 */
	trace_file_contents			contents;
	/**
		Current record of recent history.  
	 */
	trace_chunk				current_chunk;
	/**
		The cumulative size of the body of the trace-file.  
		Update this each time a chunk is flushed.  
	 */
	size_t					trace_payload_size;
	/**
		Running count of events before this chunk.  
	 */
	trace_index_type			previous_events;
public:
	static bool				notify_flush;
private:
	// for temporary construction only
	TraceManager();
public:
	explicit
	TraceManager(const string&);

	// warn if trace file is not finished!
	~TraceManager();

	bool
	good(void) const;

	operator bool () const { return good(); }

	const string&
	get_trace_name(void) const { return trace_file_name; }

	trace_index_type
	push_back_event(const event_trace_point& p) {
		return current_chunk.push_back_event(p) +previous_events;
	}

	void
	flush(void);

	/// \return number of events accumulated in since last flush
	size_t
	current_event_count(void) const {
		return current_chunk.event_count();
	}

	// write out header
	void
	finish(void);

	// text-dump?
	// load?

	static
	void
	text_dump(ifstream&, ostream&);	// we all stream for istream!

	static
	bool
	text_dump(const string&, ostream&);

};	// end class Trace

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_TRACE_H__

