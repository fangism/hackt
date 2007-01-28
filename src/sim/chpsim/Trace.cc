/**
	\file "sim/chpsim/Trace.cc"
	$Id: Trace.cc,v 1.1.2.1 2007/01/28 22:42:17 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	NOT_DONE_YET				0

#include "sim/chpsim/Trace.h"
#include <iostream>
#include <fstream>
#include "util/stacktrace.h"
#include <cstdio>			// for tmpnam

namespace HAC {
namespace SIM {
namespace CHPSIM {
//=============================================================================
// class trace_file_contents method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_file_contents::trace_file_contents() : entry_array() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
trace_file_contents::~trace_file_contents() {
}

//=============================================================================
// class TraceManager method definitions

TraceManager::TraceManager(const string& fn) : 
		trace_file_name(fn), 
		temp_file_name(tmpnam(NULL)), 		// libc/cstdio
		trace_ostream(new ofstream(temp_file_name.c_str())),
		contents(), 
		current_chunk() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TraceManager::~TraceManager() {
#if NOT_DONE_YET
	finish();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NOT_DONE_YET
bool
TraceManager::good(void) {
}
#endif

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
#if NOT_DONE_YET
	NEVER_NULL(trace_ofstream);
	// write out to temp file
	// append entry to contents
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This MUST be called to finish writing the trace.  
 */
void
TraceManager::finish(void) {
#if NOT_DONE_YET
	flush();
	// write out header to file to final file
	// concatenate trace payload (from temp.) to final file.  
#endif
}

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

