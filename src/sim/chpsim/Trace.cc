/**
	\file "sim/chpsim/Trace.cc"
	$Id: Trace.cc,v 1.2 2007/02/05 06:39:53 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
/**
	Option for the paranoid.  
	Define to 1 to plan extra sanity check alignment markers
	in the trace file, e.g. at section boundaries.  
 */
#define	CHPSIM_TRACE_ALIGNMENT_MARKERS		1

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
using std::ptr_fun;
using std::ifstream;
using std::istream_iterator;
using std::ostream_iterator;
using std::stringstream;	// string buffer
using util::write_value;
using util::read_value;
using std::streampos;
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
	typedef	event_array_type::const_iterator const_iterator;
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
	STACKTRACE_VERBOSE;
	state_trace_point_base::write(o);
	extractor_policy::write(o, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_point<Tag>::read(istream& i) {
	STACKTRACE_VERBOSE;
	state_trace_point_base::read(i);
	extractor_policy::read(i, raw_data);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_point<Tag>::dump(ostream& o) const {
	state_trace_point_base::dump(o) << '\t';
	extractor_policy::dump(o, raw_data) << endl;
	return o;
}

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
	STACKTRACE_VERBOSE;
	const size_t s = data_array.size();
	write_value(o, s);
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::write), o)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
state_trace_window_base<Tag>::read(istream& i) {
	STACKTRACE_VERBOSE;
	size_t s;
	read_value(i, s);
if (i) {
	data_array.resize(s);
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::read), i)
	);
} else {
	cerr << "Error reading data-array size." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
state_trace_window_base<Tag>::dump(ostream& o) const {
if (data_array.size()) {
	o << "\tevent\tglobal\traw\n"
		"\tindex\tindex\tvalue" << endl;
	for_each(data_array.begin(), data_array.end(), 
		bind2nd_argval(mem_fun_ref(&data_type::dump), o)
	);
}
	return o;
}

//=============================================================================
// class state_trace_time_window method definitions

void
state_trace_time_window::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	state_trace_window_base<bool_tag>::write(o);
	state_trace_window_base<int_tag>::write(o);
	state_trace_window_base<enum_tag>::write(o);
	state_trace_window_base<channel_tag>::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_trace_time_window::read(istream& i) {
	STACKTRACE_VERBOSE;
	state_trace_window_base<bool_tag>::read(i);
	if (!i) { cerr << "Error reading bool trace data." << endl; return; }
	state_trace_window_base<int_tag>::read(i);
	if (!i) { cerr << "Error reading int trace data." << endl; return; }
	state_trace_window_base<enum_tag>::read(i);
	if (!i) { cerr << "Error reading enum trace data." << endl; return; }
	state_trace_window_base<channel_tag>::read(i);
	if (!i) { cerr << "Error reading channel trace data." << endl; return; }
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
	STACKTRACE_VERBOSE;
	event_trace_window::write(o);
	state_trace_time_window::write(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_chunk::read(istream& i) {
	STACKTRACE_VERBOSE;
	event_trace_window::read(i);
if (i) {
	state_trace_time_window::read(i);
	if (!i) {
		cerr << "Error reading state-trace-window." << endl;
	}
} else {
	cerr << "Error reading event-trace-window." << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param previous_events the event offset number
 */
ostream&
trace_chunk::dump(ostream& o, const size_t previous_events) const {
	event_trace_window::dump(o, previous_events);
	state_trace_time_window::dump(o);
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
	return o << '\t' << start_time <<
		'\t' << file_offset <<
		'\t'<< chunk_size << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::entry::write(ostream& o) const {
	STACKTRACE_VERBOSE;
	write_value(o, start_time);
	write_value(o, file_offset);
	write_value(o, chunk_size);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
trace_file_contents::entry::read(istream& i) {
	STACKTRACE_VERBOSE;
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
	o << "\t\tstart\tfile\tchunk\n"
		"\tepoch\ttime\toffset\tsize" << endl;
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
// class TraceManager method definitions

/**
	I'm lazy... global flag, initialization.
 */
bool
TraceManager::notify_flush = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#if 1
		temp_file_name(tmpnam(NULL)), 		// libc/cstdio
#else
		temp_file_name(fn + "-tmp"),
#endif
		trace_ostream(new fstream(	// read and write
			temp_file_name.c_str(), 
			ios_base::binary | ios_base::trunc
				// that's silly, these should be default...
				| ios_base::in | ios_base::out
				)),
		header_ostream(new ofstream(
			trace_file_name.c_str(), ios_base::binary)), 
		contents(), 
		current_chunk(),
		trace_payload_size(0), 
		previous_events(0) {
	NEVER_NULL(trace_ostream);
	NEVER_NULL(header_ostream);
	// should always be allocated, but not necessarily in a good state
#if 0
	cerr << "Trace file name: " << trace_file_name;
	cerr << ((*header_ostream) ? " (opened)" : " (failed)") << endl;
	cerr << "Temp file name: " << temp_file_name;
	cerr << ((*trace_ostream) ? " (opened)" : " (failed)") << endl;
#endif
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
	if (notify_flush) {
		cout << "trace manager: flushing out another epoch of trace."
			<< endl;
	}
	const trace_time_type start_time = current_chunk.start_time();
	const streampos old_size = trace_ostream->tellp();
	current_chunk.write(*trace_ostream);
	trace_ostream->flush();
	trace_payload_size = trace_ostream->tellp();
	STACKTRACE_INDENT_PRINT("chunk written to offsets [" << old_size <<
		':' << trace_payload_size << "]." << endl);
	// append entry to contents
	contents.push_back(trace_file_contents::entry(
		start_time, old_size, trace_payload_size -old_size));
	// restart chunk, recycling memory (placement dtor and ctor)
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
if (good()) {
	flush();	// one last flush (if needed)
	// write out header to file to final file
	NEVER_NULL(header_ostream);
	contents.write(*header_ostream);
	// align to natural boundary? maybe sanity check code.
	const streampos start_of_objects = header_ostream->tellp();
	STACKTRACE_INDENT_PRINT("header written up to offset: "
		<< start_of_objects << endl);
#if CHPSIM_TRACE_ALIGNMENT_MARKERS
	static const size_t marker = 0xFFFFFFFF;
	write_value(*header_ostream, marker);
	STACKTRACE_INDENT_PRINT("starting body at: "
		<< header_ostream->tellp() << endl);
#endif
	// concatenate trace payload (from temp.) to final file.  
{
	// std::copy using stream iterators will NOT work
	static const size_t buf_size = 1<<12;	// stack buffer size
	char buf[buf_size];	// stack big enough?
	size_t trace_end = trace_ostream->tellp();
	STACKTRACE_INDENT_PRINT("trace body ends at: " << trace_end << endl);
	trace_ostream->seekg(0);
	while (trace_end > buf_size) {
		// massive transfer
		trace_ostream->read(buf, buf_size);
		header_ostream->write(buf, buf_size);
		trace_end -= buf_size;
	}
	// cleanup
	trace_ostream->read(buf, trace_end);
	header_ostream->write(buf, trace_end);
	const streampos end_of_objects = header_ostream->tellp();
	STACKTRACE_INDENT_PRINT("payload finished at offset: " <<
		end_of_objects << endl);
}
	trace_ostream = excl_ptr<fstream>(NULL);
	header_ostream = excl_ptr<ofstream>(NULL);
#if 0
	// remove the temporary file if it wasn't generated by tmpnam
	remove(temp_file_name.c_str());		// libc/stdio
	// don't care about exit status?
#endif
	if (notify_flush) {
		cout << "trace manager: trace file complete." << endl;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true to signal error opening file. 
 */
bool
TraceManager::text_dump(const string& fn, ostream& o) {
	ifstream f(fn.c_str(), ios_base::binary);
	if (f) {
		text_dump(f, o);
		return false;
	} else {
		return true;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads in trace file and produces a massive text dump, which you're
	supposed to correlate with the CHP event graph.  Fun.  
	This is the reverse of the finish() method.  
	\param i binary input stream of the trace file
	\param o text output stream for human(?) grokability.  
 */
void
TraceManager::text_dump(ifstream& i, ostream& o) {
#if 0
	INVARIANT(i.flags() & ios_base::binary);
	INVARIANT(!(o.flags() & ios_base::binary));
#endif
	TraceManager tm;
	// reverse the process of ::finish()
	tm.contents.read(i);
if (i) {
#if ENABLE_STACKTRACE
	const streampos start_of_objects = i.tellg();
	cerr << "header read up to offset: " << start_of_objects<< endl;
#endif
	tm.contents.dump(o);
	// note header offset? start of payload?
#if CHPSIM_TRACE_ALIGNMENT_MARKERS
{
	size_t check;
	read_value(i, check);
	INVARIANT(check == 0xFFFFFFFF);
}
#endif
//	o << "Trace events and data, by epoch:" << endl;
	size_t j = 0;
	trace_file_contents::const_iterator
		ci(tm.contents.begin()), ce(tm.contents.end());
	for ( ; ci!=ce; ++ci, ++j) {
		o << "Epoch " << j << ':' << endl;
#if ENABLE_STACKTRACE
		const streampos head = i.tellg();
#endif
		tm.current_chunk.read(i);
		if (i) {
#if ENABLE_STACKTRACE
			const streampos tail = i.tellg();
			cerr << "read chunk of size: " << tail -head << endl;
#endif
			tm.current_chunk.dump(o, tm.previous_events);
			tm.previous_events += tm.current_event_count();
		} else {
			cerr << "Error encountered in reading trace payload!"
				<< endl;
			break;
		}
	}
} else {
	cerr << "Error encountered in reading trace header!" << endl;
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

