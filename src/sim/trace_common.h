/**
	\file "sim/trace_common.h"
	$Id: trace_common.h,v 1.1.2.4 2009/01/27 22:16:36 fang Exp $
	Generic simulation execution trace structures.  
 */

#ifndef	__HAC_SIM_TRACE_COMMON_H__
#define	__HAC_SIM_TRACE_COMMON_H__

#include "sim/time.h"
#include "sim/common.h"
#include "util/attributes.h"
#include <iosfwd>
#include <vector>
#include <string>
#include "util/memory/excl_ptr.h"

/**
	Option for the paranoid.  
	Define to 1 to plan extra sanity check alignment markers
	in the trace file, e.g. at section boundaries.  
 */
#define	CHPSIM_TRACE_ALIGNMENT_MARKERS		1

namespace HAC {
namespace SIM {
using std::istream;
using std::ostream;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using util::memory::excl_ptr;

/**
	NOTE: keep consistent with EventNode::time_type
 */
typedef	real_time			trace_time_type;
/**
	Might have to use 64b for large traces...
 */
typedef	event_index_type		trace_index_type;

enum {
	INVALID_TRACE_INDEX = 0
};

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
			const trace_index_type c = INVALID_TRACE_INDEX) :
			timestamp(t), event_id(ei), cause_id(c)
			{ }

	void
	write(ostream&) const;

	void
	read(istream&);

	ostream&
	__dump(ostream&) const;

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
	typedef	event_array_type::const_iterator	const_iterator;

	bool
	empty(void) const { return event_array.empty(); }

	const_iterator
	begin(void) const { return event_array.begin(); }

	const_iterator
	end(void) const { return event_array.end(); }

	/// unchecked local event index, use sparingly
	const event_trace_point&
	get_event(const size_t ei) const {
		return event_array[ei];
	}

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

	/**
		Ordering comparator.  
		Since data_array is appended in increasing order of
		event_indices.  
	 */
	struct event_index_less_than {
		bool
		operator () (const state_trace_point_base& l, 
			const trace_index_type r) const {
			return l.event_index < r;
		}

		bool
		operator () (const trace_index_type l,
			const state_trace_point_base& r) {
			return l < r.event_index;
		}
	};	// end struct event_index_less_than
};	// end struct state_trace_point_base

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
		Each entry represents an epoch (chunk).  
	 */
	struct entry {
		/**
			The index of the first event in an epoch.  
		 */
		size_t				start_index;
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
		entry(const size_t i, 
			const trace_time_type t, const size_t o, 
			const size_t s) :
			start_index(i), 
			start_time(t), file_offset(o), chunk_size(s) { }

		// human readable
		ostream&
		dump(ostream&) const;

		void
		write(ostream&) const;

		void
		read(istream&);

		/**
			Index comparator functor, for binary search.
			See std::lower_bound for use context.
		 */
		struct event_index_less_than {
			bool
			operator () (const entry& e, const size_t i) const {
				return e.start_index < i;
			}

			bool
			operator () (const size_t i, const entry& e) const {
				return i < e.start_index;
			}
		};	// end struct event_index_less_than
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

	bool
	empty(void) const { return entry_array.empty(); }

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
/**
	Minimal trace information to reproduce all information.
	Trace should be replayable.
	TODO: trace consistency and integrity checks.  
 */
class trace_manager_base {
protected:
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
		This is the first section written to the trace file
		that is a directory for the entire trace file.
	 */
	trace_file_contents			contents;
	/**
		The cumulative size of the body of the trace-file.  
		Update this each time a chunk is flushed.  
	 */
	size_t					trace_payload_size;
	/**
		Running count of events before this chunk.  
		Update this value each time a chunk is flushed out.
	 */
	trace_index_type			previous_events;
public:
	static bool				notify_flush;
protected:
	// for temporary construction only
	trace_manager_base();
public:
	explicit
	trace_manager_base(const string&);

	// warn if trace file is not finished!
	~trace_manager_base();

	bool
	good(void) const;

	operator bool () const { return good(); }

	const string&
	get_trace_name(void) const { return trace_file_name; }

	trace_index_type
	get_previous_events(void) const {
		return previous_events;
	}

#if 0
	// text-dump?
	// load?

	static
	void
	text_dump(ifstream&, ostream&);	// we all stream for istream!

	static
	bool
	text_dump(const string&, ostream&);
#endif

private:
	// write out header
	void
	__finish(void);

};	// end class trace_manager_base

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_TRACE_COMMON_H__


