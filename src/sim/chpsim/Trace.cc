/**
	\file "sim/chpsim/Trace.cc"
	$Id: Trace.cc,v 1.1.2.6 2007/01/31 20:59:29 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	NOT_DONE_YET				0

#include "sim/chpsim/Trace.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <fstream>
#include "util/stacktrace.h"
#include <cstdio>			// for tmpnam
#include "util/IO_utils.tcc"	// .tcc?
#include "util/binders.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::for_each;
using std::ios_base;
using std::copy;
using std::istream_iterator;
using std::ostream_iterator;
using std::stringstream;	// string buffer
using util::write_value;
using util::read_value;
using std::streampos;
using std::mem_fun_ref;
using util::bind2nd_argval;

//=============================================================================
// class event_trace_point method definitions

/**
	Private ostream overload, local to this TU.  
 */
void
event_trace_point::write(ostream& o) const {
	write_value(o, timestamp);
	write_value(o, event_id);
#if CHPSIM_CAUSE_TRACKING
	write_value(o, cause_id);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
event_trace_point::read(istream& i) {
	read_value(i, timestamp);
	read_value(i, event_id);
#if CHPSIM_CAUSE_TRACKING
	read_value(i, cause_id);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
event_trace_point::dump(ostream& o) const {
	o << "time: " << timestamp;
	o << ", event: " << event_id;
#if CHPSIM_CAUSE_TRACKING
	o << ", cause: " << cause_id;
#endif
	return o;
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
#if 0
	ostream_iterator<event_trace_point> osi(o);
	copy(event_array.begin(), event_array.end(), osi);
#else
	const size_t s = event_array.size();
	write_value(o, s);
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::write), o)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
event_trace_window::read(istream& i) {
	size_t s;
	read_value(i, s);
	event_array.resize(s);
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::read), i)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
event_trace_window::dump(ostream& o) const {
	for_each(event_array.begin(), event_array.end(), 
		bind2nd_argval(mem_fun_ref(&event_trace_point::dump), o)
	);
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
	o << "trace_event[" << event_index << "], alloc-index=" << global_index;
	return o;
}

//=============================================================================
// class state_trace_point method definitions


template <class Tag>
state_trace_point<Tag>::state_trace_point(const value_type& d, 
		const trace_index_type t, const size_t g) :
		state_trace_point_base(t, g), 
		raw_data(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_point<Tag>::write(ostream& o) const {
	state_trace_point_base::write(o);
	extractor_policy::write(o, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_point<Tag>::read(istream& i) {
	state_trace_point_base::read(i);
	extractor_policy::read(i, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_point<Tag>::dump(ostream& o) const {
	state_trace_point_base::dump(o) << " touched: ";
	extractor_policy::dump(o, raw_data) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Private ostream overload, local to this TU.  
 */
template <class Tag>
static
ostream&
operator << (ostream& o, const state_trace_point<Tag>& p) {
	p.write(o);
	return o;
}
#endif

//=============================================================================
// class state_trace_window_base method definitions

/**
	Extracts the relevant data to save to trace and stores it in a buffer.
 */
template <class Tag>
void
state_trace_window_base<Tag>::__push_back(const var_type& v, 
		const trace_index_type t, const size_t g) {
	data_array.push_back(data_type(extractor_policy()(v), t, g));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now we just write out the entire vector brainlessly.
	TODO: (enhancement) group by like-indexed reference.  
	Or use global-indexed map of vectors.
 */
template <class Tag>
void
state_trace_window_base<Tag>::write(ostream& o) const {
	const size_t s = data_array.size();
	write_value(o, s);
#if 0
	ostream_iterator<data_type> osi(o);
	copy(data_array.begin(), data_array.end(), osi);
#else
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::write), o)
	);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_window_base<Tag>::read(istream& i) {
	size_t s;
	read_value(i, s);
	data_array.resize(s);
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::read), i)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_window_base<Tag>::dump(ostream& o) const {
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::dump), o)
	);
	return o;
}

//=============================================================================
// class state_trace_time_window method definitions

void
state_trace_time_window::write(ostream& o) const {
	state_trace_window_base<bool_tag>::write(o);
	state_trace_window_base<int_tag>::write(o);
	state_trace_window_base<enum_tag>::write(o);
	state_trace_window_base<channel_tag>::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_trace_time_window::read(istream& i) {
	state_trace_window_base<bool_tag>::read(i);
	state_trace_window_base<int_tag>::read(i);
	state_trace_window_base<enum_tag>::read(i);
	state_trace_window_base<channel_tag>::read(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
state_trace_time_window::dump(ostream& o) const {
	o << "bool state trace:" << endl;
	state_trace_window_base<bool_tag>::dump(o);
	o << "int state trace:" << endl;
	state_trace_window_base<int_tag>::dump(o);
	o << "enum state trace:" << endl;
	state_trace_window_base<enum_tag>::dump(o);
	o << "channel state trace:" << endl;
	state_trace_window_base<channel_tag>::dump(o);
	return o;
}

//=============================================================================
// class trace_chunk method definitions

trace_chunk::trace_chunk() : 
		state_trace_time_window(), 
		event_trace_window() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_chunk::~trace_chunk() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-destructive write-out of contents.  
 */
void
trace_chunk::write(ostream& o) const {
	event_trace_window::write(o);
	state_trace_time_window::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_chunk::read(istream& i) {
	event_trace_window::read(i);
	state_trace_time_window::read(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
trace_chunk::dump(ostream& o) const {
	event_trace_window::dump(o);
	state_trace_time_window::dump(o);
	return o;
}

//=============================================================================
// class trace_file_contents method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private ostream overload, local to this TU.  
	I must apologize, this is blatant disregard of:
	http://gcc.gnu.org/onlinedocs/libstdc++/27_io/howto.html
 */
static
ostream&
operator << (ostream& o, const trace_file_contents::entry& e) {
if (o.flags() & ios_base::binary) {
	write_value(o, e.start_time);
	write_value(o, e.file_offset);
	write_value(o, e.chunk_size);
} else {
	o << "start time: " << e.start_time <<
		"\toffset: " << e.file_offset <<
		"\tchunk size: " << e.chunk_size << endl;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
istream&
trace_file_contents::entry::read(istream& i) {
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
	INVARIANT(!(o.flags() & ios_base::binary));
	copy(entry_array.begin(), entry_array.end(),
		ostream_iterator<entry>(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::write(ostream& o) const {
	INVARIANT(o.flags() & ios_base::binary);
	const size_t s = entry_array.size();
	write_value(o, s);
	ostream_iterator<entry> osi(o);
	copy(entry_array.begin(), entry_array.end(), osi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::read(istream& i) {
	size_t s;
	read_value(i, s);
	entry_array.resize(s);
	for_each(entry_array.begin(), entry_array.end(), 
		bind2nd_argval(mem_fun_ref(&entry::read), i)
	);	// give me boost::lambda!
#if 0
	for_each(entry_array.begin(), entry_array.end(),
		bind(&entry::read, _1, ref(i)));
#endif
}

//=============================================================================
// class TraceManager method definitions

/**
	Private default constructor, only used to construct a temporary.  
 */
TraceManager::TraceManager() : 
		trace_file_name(), 
		temp_file_name(),
		trace_ostream(NULL), 
		header_ostream(NULL), 
		contents(), 
		current_chunk(),
		trace_payload_size(0), 
		previous_events(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Opens the requested file streams in binary mode for writing.  
	Caller should check good() immediately after construction.  
 */
TraceManager::TraceManager(const string& fn) : 
		trace_file_name(fn), 
		temp_file_name(tmpnam(NULL)), 		// libc/cstdio
		trace_ostream(new fstream(	// read and write
			temp_file_name.c_str(), ios_base::binary)),
		header_ostream(new ofstream(fn.c_str(), ios_base::binary)), 
		contents(), 
		current_chunk(),
		trace_payload_size(0), 
		previous_events(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TraceManager::~TraceManager() {
	finish();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if both output streams are good to go.  
 */
bool
TraceManager::good(void) const {
	return trace_ostream && header_ostream &&
		*trace_ostream && *header_ostream;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out the current contents of the chunk to (temp) file.  
	Append an entry to the table of contents.  
	This also destructively resets the current chunk 
	(releasing memory) so new data can be appended.  
	With regular flushing, memory usage can be capped.  
 */
void
TraceManager::flush(void) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(trace_ostream);
	// write out to temp file
	// INVARIANT, there must be at least ONE event in the trace
	// otherwise, this flush is just wasted (no-op)
if (current_chunk.event_count()) {
	const trace_time_type start_time = current_chunk.start_time();
	const streampos old_size = trace_ostream->tellp();
	current_chunk.write(*trace_ostream);
	trace_payload_size = trace_ostream->tellp();
	// append entry to contents
	contents.push_back(trace_file_contents::entry(
		start_time, old_size, trace_payload_size -old_size));
	// restart chunk, recycling memory
	current_chunk.~trace_chunk();
	new (&current_chunk) trace_chunk();
}
	// else there is nothing to flush
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This MUST be called to finish writing the trace.  
 */
void
TraceManager::finish(void) {
	STACKTRACE_VERBOSE;
	flush();	// one last flush (if needed)
	// write out header to file to final file
	contents.write(*header_ostream);
	const streampos start_of_objects = header_ostream->tellp();
	// align to natural boundary? maybe sanity check code.
	// concatenate trace payload (from temp.) to final file.  
	copy(istream_iterator<char>(*trace_ostream), istream_iterator<char>(),
		ostream_iterator<char>(*header_ostream));
	// close both streams when done (will cause flush)
	// close stream and release memory
	trace_ostream = excl_ptr<fstream>(NULL);
	header_ostream = excl_ptr<ofstream>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in trace file and produces a massive text dump, which you're
	supposed to correlate with the CHP event graph.  Fun.  
	\param i binary input stream of the trace file
	\param o text output stream for human(?) grokability.  
 */
void
TraceManager::text_dump(istream& i, ostream& o) {
	INVARIANT(i.flags() & ios_base::binary);
	INVARIANT(!(o.flags() & ios_base::binary));
	TraceManager tm;
	// reverse the process of ::finish()
	tm.contents.read(i);
	// note header offset? start of payload?
	trace_file_contents::const_iterator
		ci(tm.contents.begin()), ce(tm.contents.end());
	for ( ; ci!=ce; ++ci) {
		tm.current_chunk.read(i);
		tm.current_chunk.dump(o);
	}
}

//=============================================================================
// explicit template instantiations

template class state_trace_window_base<bool_tag>;
template class state_trace_window_base<int_tag>;
template class state_trace_window_base<enum_tag>;
template class state_trace_window_base<channel_tag>;

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

