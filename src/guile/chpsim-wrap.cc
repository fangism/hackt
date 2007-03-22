/**
	\file "guile/chpsim-wrap.cc"
	$Id: chpsim-wrap.cc,v 1.2.2.3 2007/03/22 05:17:46 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "guile/chpsim-wrap.h"
#include "guile/libhackt-wrap.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/Trace.h"
// #include "sim/chpsim/graph_options.h"
#include "util/stacktrace.h"
#include "util/libguile.h"
#include "util/guile_STL.h"
#include "guile/scm_chpsim_trace_streamer.h"

namespace HAC {
namespace guile_wrap {
using SIM::CHPSIM::State;
using SIM::CHPSIM::TraceManager;
using SIM::CHPSIM::event_trace_point;
// using SIM::CHPSIM::graph_options;
#include "util/using_ostream.h"
using util::guile::scm_assert_string;
using util::guile::make_scm;

//=============================================================================
/**
	The single global state object used by guile chpsim extensions.
	Set this pointer before calling any functions in this library.  
	Requires that the module object be established too...
	NOTE: this object requires a valid obj_module module reference.  
 */
count_ptr<State> chpsim_state(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if HAVE_ATEXIT
/**
	After main(), before global static dtors, release some resources
	explicitly because guile's shell does not return.
 */
static
void
release_chpsim_wrap_resources_at_exit(void) {
	STACKTRACE_VERBOSE;
	if (chpsim_state) {
		chpsim_state = count_ptr<State>(NULL);
		// will cause deallocation upon last reference
		// there shouldn't be any other references, assert?
	}
}
#endif	// HAVE_ATEXIT

//-----------------------------------------------------------------------------
/**
	Prints dump of dot structure to stdout.  
	\return nothing
	TODO: somehow pass options to it, SCM arguments?
 */
static
SCM
wrap_chpsim_dump_graph_alloc(void) {
#define	FUNC_NAME "dump-chpsim-struct"
	NEVER_NULL(chpsim_state);
//	graph_options dflt;
	chpsim_state->dump_struct(cout);
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints text dump of chpsim trace fo stdout.
	\param s_str the name of trace file.
 */
static
SCM
wrap_chpsim_dump_trace(SCM s_str) {
#define	FUNC_NAME "dump-trace"
	scm_assert_string(s_str, FUNC_NAME, 1);
	const char* tf = scm_to_locale_string(s_str);
	if (TraceManager::text_dump(tf, cout)) {
		cerr << "Error opening trace file: " << tf << endl;
	}
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_str the name of the tracefile (string)
	\return smob of the newly opened tracefile.
 */
static
SCM
wrap_open_chpsim_trace(SCM s_str) {
	STACKTRACE_VERBOSE;
#define	FUNC_NAME "open-chpsim-trace"
	const std::string peek(scm_to_locale_string(s_str));	// 1.8
	// alternately string_to_locale_stringbuf
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<scm_chpsim_trace_stream>
		tf(new scm_chpsim_trace_stream(peek));
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_chpsim_trace_stream_tag, tf.release());
	return ret_smob;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads an entry AND advances one position.
	\param s_tr a smob corresponding to the opened trace file that
		we're streaming from.
	\return scm object representing entry, or end-of-stream marker
		which is null (SCM_EOL).  
 */
static
SCM
wrap_chpsim_trace_entry_to_scm(SCM s_tr) {
	STACKTRACE_VERBOSE;
#define	FUNC_NAME "current-trace-entry"
	scm_assert_smob_type(raw_chpsim_trace_stream_tag, s_tr);
	scm_chpsim_trace_stream* const ptr =
		reinterpret_cast<scm_chpsim_trace_stream*>(SCM_SMOB_DATA(s_tr));
	if (!(ptr && ptr->good())) {
#if 0
		scm_misc_error(FUNC_NAME, 
			"Error: invalid trace stream state.",
			SCM_EOL);
#else
		return SCM_EOL;
#endif
	}
	const event_trace_point& tp(ptr->current_event_record());
	const SCM ret = scm_cons(make_scm(ptr->index()), 
		scm_cons(make_scm(tp.timestamp), 
		scm_cons(make_scm(tp.event_id),
			make_scm(tp.cause_id))));
	// alternatively, last pair can be made with SCM_EOL
	ptr->advance();	// should never fail, really...
	return ret;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if the trace-stream is still valid.  
 */
static
SCM
wrap_chpsim_trace_stream_valid_p(SCM s_tr) {
	STACKTRACE_VERBOSE;
#define	FUNC_NAME "chpsim-trace-valid?"
	scm_assert_smob_type(raw_chpsim_trace_stream_tag, s_tr);
	const scm_chpsim_trace_stream* const ptr =
		reinterpret_cast<scm_chpsim_trace_stream*>(SCM_SMOB_DATA(s_tr));
	return make_scm<bool>(ptr && ptr->good());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================
BEGIN_C_DECLS
using namespace HAC::guile_wrap;
using util::guile::scm_gsubr_type;

/**
	Registers shared library functions for to guile interpreter.
	TODO: namespace-ing exported functions?
 */
static
void
__libhacktsim_guile_init(void* unused) {
	NEVER_NULL(chpsim_state);
	libhackt_guile_init();		// prerequisite module
	raw_chpsim_trace_stream_smob_init();
	// (use-modules (ice-9 streams))?
	// initialize any smob types we use
	scm_c_define_gsubr("dump-chpsim-struct", 0, 0, 0, 
		wrap_chpsim_dump_graph_alloc);
	scm_c_define_gsubr("dump-trace", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_dump_trace));
	scm_c_define_gsubr("open-chpsim-trace", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_open_chpsim_trace));
	scm_c_define_gsubr("chpsim-trace-valid?", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_trace_stream_valid_p));
	scm_c_define_gsubr("current-trace-entry", 1, 0, 0,
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_trace_entry_to_scm));
	// export as part of module interface
	scm_c_export("dump-chpsim-struct", NULL);
	scm_c_export("dump-trace", NULL);
	scm_c_export("open-chpsim-trace", NULL);
	scm_c_export("chpsim-trace-valid?", NULL);
	scm_c_export("current-trace-entry", NULL);
#if HAVE_ATEXIT
	const int x = atexit(release_chpsim_wrap_resources_at_exit);
	INVARIANT(!x);
#endif
}	// end libhacktsim_guile_init

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register interface function, wrapped call.  
 */
void
libhacktsim_guile_init(void) {
	__libhacktsim_guile_init(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to load the functions into a module.  
 */
void
scm_init_hackt_chpsim_trace_primitives_module(void) {
	scm_c_define_module("hackt chpsim-trace-primitives",
		__libhacktsim_guile_init, NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
END_C_DECLS

