/**
	\file "sim/trace_common.cc"
	$Id: trace_common.cc,v 1.1.2.1 2009/01/20 02:57:02 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "sim/trace_common.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <fstream>
#include "util/stacktrace.h"
#include <cstdio>			// for tmpnam
#include "util/libc_temp.h"	// for temp file functions
#include "util/IO_utils.tcc"	// .tcc?
#include "util/binders.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"
using std::for_each;
using std::ifstream;
using util::write_value;
using util::read_value;
using std::mem_fun_ref;
using util::bind1st_argval;
using util::bind2nd_argval;

//=============================================================================
// class event_trace_point method definitions

/**
	Private ostream overload, local to this TU.  
 */
void
event_trace_point::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	write_value(o, timestamp);
	write_value(o, event_id);
	write_value(o, cause_id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
event_trace_point::read(istream& i) {
	STACKTRACE_VERBOSE;
	read_value(i, timestamp);
	read_value(i, event_id);
	read_value(i, cause_id);
	if (!i) {
		cerr << "Error reading event trace point." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
event_trace_point::dump(ostream& o) const {
	o << '\t' << timestamp << '\t' << event_id;
	o << "\t" << cause_id;
	return o << endl;
}

//=============================================================================
// class event_trace_window method definitions

/**
	\return the index of the new traced event, so that other
		trace data may reference this to get its timestamp.  
 */
trace_index_type
event_trace_window::push_back_event(const event_trace_point& p) {
	const trace_index_type ret = event_array.size();
	event_array.push_back(p);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre event array must not be empty.  
 */
trace_time_type
event_trace_window::start_time(void) const {
	INVARIANT(event_array.size());
	return event_array.front().timestamp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre event array must not be empty.  
 */
trace_time_type
event_trace_window::end_time(void) const {
	INVARIANT(event_array.size());
	return event_array.back().timestamp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out event (binary).  
 */
void
event_trace_window::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	const size_t s = event_array.size();
	write_value(o, s);
	STACKTRACE_INDENT_PRINT("event-window has " << s << " events." << endl);
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::write), o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
event_trace_window::read(istream& i) {
	STACKTRACE_VERBOSE;
	size_t s;
	read_value(i, s);
if (i) {
	event_array.resize(s);
	STACKTRACE_INDENT_PRINT("event-window has " << s << " events." << endl);
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::read), i)
	);
} else {
	cerr << "Error reading event-trace-window size." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints table of events ordered by index of occurrence.  
 */
ostream&
event_trace_window::dump(ostream& o, const size_t offset) const {
	o << "\tevent\t\tevent\tcause" << endl;
	o << "\tindex\ttime\tnode\tindex" << endl;
#if 0
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::dump), o)
	);
#else
	const_iterator i(event_array.begin()), e(event_array.end());
	size_t j = offset;
	for ( ; i!=e; ++i, ++j) {
		i->dump(o << '\t' << j);	// has endl already
	}
#endif
	return o;
}

//=============================================================================
// class state_trace_point_base method definitions

void
state_trace_point_base::write(ostream& o) const {
	write_value(o, event_index);
	write_value(o, global_index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_trace_point_base::read(istream& i) {
	read_value(i, event_index);
	read_value(i, global_index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_trace_point_base::dump(ostream& o) const {
	o << '\t' << event_index << '\t' << global_index;
	return o;
}

//=============================================================================
// class trace_file_contents method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
trace_file_contents::entry::dump(ostream& o) const {
#if 0
	o << "start time: " << start_time <<
		"\toffset: " << file_offset <<
		"\tchunk size: " << chunk_size << endl;
	return o;
#else
	o << '\t' << start_index <<
		'\t' << start_time <<
		'\t' << file_offset <<
		'\t'<< chunk_size << endl;
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::entry::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	write_value(o, start_index);
	write_value(o, start_time);
	write_value(o, file_offset);
	write_value(o, chunk_size);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::entry::read(istream& i) {
	STACKTRACE_VERBOSE;
	read_value(i, start_index);
	read_value(i, start_time);
	read_value(i, file_offset);
	read_value(i, chunk_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_file_contents::trace_file_contents() : entry_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_file_contents::~trace_file_contents() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre o must be in text mode (not binary)
 */
ostream&
trace_file_contents::dump(ostream& o) const {
	o << "Trace file contents: " << entry_array.size() <<
		" epochs." << endl;
	o << "\t\tstart\tstart\tfile\tchunk\n"
		"\tepoch\tindex\ttime\toffset\tsize" << endl;
#if 0
	for_each(entry_array.begin(), entry_array.end(),
		bind2nd_argval(mem_fun_ref(&entry::dump), o)
	);
#else
	size_t j = 0;
	const_iterator i(entry_array.begin()), e(entry_array.end());
	for ( ; i!=e; ++i, ++j) {
		i->dump(o << '\t' << j);
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	const size_t s = entry_array.size();
	write_value(o, s);
	STACKTRACE_INDENT_PRINT("contents: " << s << " entries" << endl);
	for_each(entry_array.begin(), entry_array.end(),
		bind2nd_argval(mem_fun_ref(&entry::write), o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::read(istream& i) {
	STACKTRACE_VERBOSE;
	size_t s;
	read_value(i, s);
if (i) {
	STACKTRACE_INDENT_PRINT("contents: " << s << " entries" << endl);
	entry_array.resize(s);
	for_each(entry_array.begin(), entry_array.end(), 
		bind2nd_argval(mem_fun_ref(&entry::read), i)
	);	// give me boost::lambda!
#if 0
	for_each(entry_array.begin(), entry_array.end(),
		bind(&entry::read, _1, ref(i)));
#endif
} else {
	cerr << "Error reading contents size." << endl;
}
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

