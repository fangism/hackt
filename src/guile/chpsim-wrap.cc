/**
	\file "guile/chpsim-wrap.cc"
	$Id: chpsim-wrap.cc,v 1.2.2.10 2007/04/04 04:31:25 fang Exp $
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
#include "guile/scm_chpsim_event_node.h"
#include "guile/hackt-documentation.h"
#include "util/for_all.h"
#include "util/caller.h"

namespace HAC {
namespace guile_wrap {
using SIM::CHPSIM::State;
using SIM::CHPSIM::TraceManager;
using SIM::CHPSIM::event_trace_point;
// using SIM::CHPSIM::graph_options;
#include "util/using_ostream.h"
using util::guile::scm_assert_string;
using util::guile::make_scm;
using util::guile::extract_scm;
using util::guile::scm_c_define_gsubr_exported;

//=============================================================================
/**
	The single global state object used by guile chpsim extensions.
	Set this pointer before calling any functions in this library.  
	Requires that the module object be established too...
	NOTE: this object requires a valid obj_module module reference.  
 */
count_ptr<State> chpsim_state(NULL);

/**
	Local SCM function initialization registry.  
 */
static
std::vector<scm_init_func_type>		local_chpsim_registry;

static
std::vector<scm_init_func_type>		local_chpsim_trace_registry;

/**
        Don't use line-continuation right before ARGLIST, triggers cpp bug?
 */
#define HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, 	\
		REGISTRY, DOCSTRING)					\
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT,			\
	VAR, ARGLIST, REGISTRY, DOCSTRING)

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
	Re-usable constant symbol, can use this in C,C++.  
	Saves symbol creation and replication.  
 */
// HAC_GUILE_SYMBOL(symbol_ack, "ack");

//-----------------------------------------------------------------------------
/**
	Prints dump of dot structure to stdout.  
	\return nothing
	TODO: somehow pass options to it, SCM arguments?
 */
#define	FUNC_NAME "dump-chpsim-struct"
HAC_GUILE_DEFINE(wrap_chpsim_dump_graph_alloc, FUNC_NAME, 0, 0, 0, (void),
	local_chpsim_registry, 
"Produces textual dump of chpsim\'s allocated state structure to stdout, "
"like \"chpsim -fno-run -fdump-struct\".") {
	NEVER_NULL(chpsim_state);
//	graph_options dflt;
	chpsim_state->dump_struct(cout);
	return SCM_UNSPECIFIED;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct an SCM struct from a single event entry from trace.
	MAINTAINENCE: Keep this consistent with "scm/hackt/chpsim.scm"
 */
static
SCM
scm_from_event_trace_point(const event_trace_point& tp, const size_t i) {
	return scm_cons(make_scm(i), 
		scm_cons(make_scm(tp.timestamp), 
		scm_cons(make_scm(tp.event_id),
			make_scm(tp.cause_id))));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Converts current state-change entry into SCM object.
	Basically contains records of reference-value tuples.  
 */
static
SCM
scm_from_state_trace_point(
		const TraceManager::state_change_streamer::
			pseudo_const_iterator_range& tp, 
		const size_t i) {
	// TODO: construct structure of references, FINISH_ME
	return scm_cons(make_scm(i), SCM_EOL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads an entry AND advances one position.
	This call is usually memoized to avoid accidentally calling twice
	at the same position.  
	\param strm a smob corresponding to the opened trace file that
		we're streaming from.
	\return scm object representing entry, or end-of-stream marker
		which is null (SCM_EOL).  
 */
#define	FUNC_NAME "current-trace-entry"
HAC_GUILE_DEFINE(wrap_chpsim_trace_entry_to_scm, FUNC_NAME, 1, 0, 0, 
	(SCM strm), local_chpsim_trace_registry, 
"Interprets the current event-trace entry of the (smob) trace stream @var{str} "
"*and* advances one position in the stream.") {
	STACKTRACE_VERBOSE;
	scm_assert_smob_type(raw_chpsim_trace_stream_tag, strm);
	scm_chpsim_trace_stream* const ptr =
		scm_smob_to_chpsim_trace_stream_ptr(strm);
	if (!(ptr && ptr->good())) {
		return SCM_EOL;
	}
	const event_trace_point& tp(ptr->current_event_record());
	const SCM ret = scm_from_event_trace_point(tp, ptr->index());
	// alternatively, last pair can be made with SCM_EOL
	ptr->advance();	// should never fail, really...
	return ret;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads an entry AND retreats one position (reverse stream).
	This call is usually memoized to avoid accidentally calling twice
	at the same position.  
	\param strm a smob corresponding to the opened trace file that
		we're reverse-streaming from.
	\return scm object representing entry, or end-of-stream marker
		which is null (SCM_EOL).  
 */
#define	FUNC_NAME "current-trace-reverse-entry"
HAC_GUILE_DEFINE(wrap_chpsim_reverse_trace_entry_to_scm, FUNC_NAME, 1, 0, 0, 
	(SCM strm), local_chpsim_trace_registry, 
"Interprets the current event-trace entry of the (smob) trace reverse stream "
"@var{str} *and* retreats one position in the stream.") {
	STACKTRACE_VERBOSE;
	scm_assert_smob_type(raw_chpsim_trace_reverse_stream_tag, strm);
	scm_chpsim_trace_reverse_stream* const ptr =
		scm_smob_to_chpsim_trace_reverse_stream_ptr(strm);
	if (!(ptr && ptr->good())) {
		return SCM_EOL;
	}
	const event_trace_point& tp(ptr->current_event_record());
	const SCM ret = scm_from_event_trace_point(tp, ptr->index());
	// alternatively, last pair can be made with SCM_EOL
	ptr->retreat();	// should never fail, really...
	return ret;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads an state-change entry AND advances one position.
	This call is usually memoized to avoid accidentally calling twice
	at the same position.  
	\param strm a smob corresponding to the opened trace file that
		we're streaming from.
	\return scm object representing entry, or end-of-stream marker
		which is null (SCM_EOL).  
 */
#define	FUNC_NAME "current-state-trace-entry"
HAC_GUILE_DEFINE(wrap_chpsim_state_change_trace_entry_to_scm, 
	FUNC_NAME, 1, 0, 0, 
	(SCM strm), local_chpsim_trace_registry, 
"Interprets the current state-trace entry of the (smob) trace stream @var{str} "
"*and* advances one position in the stream.") {
	STACKTRACE_VERBOSE;
	scm_assert_smob_type(raw_chpsim_state_change_stream_tag, strm);
	scm_chpsim_state_change_stream* const ptr =
		scm_smob_to_chpsim_state_change_stream_ptr(strm);
	if (!(ptr && ptr->good())) {
		return SCM_EOL;
	}
	const TraceManager::state_change_streamer::pseudo_const_iterator_range&
		tp(ptr->current_state_iter());
	const SCM ret = scm_from_state_trace_point(tp, ptr->index());
	// alternatively, last pair can be made with SCM_EOL
	ptr->advance();	// should never fail, really...
	return ret;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param tr the chpsim trace, random-access handle.
	\param ind the dynamic event to access, NOT the static event.
 */
#define	FUNC_NAME "lookup-trace-entry"
HAC_GUILE_DEFINE(wrap_chpsim_lookup_trace_entry, 
	FUNC_NAME, 2, 0, 0, (SCM tr, SCM ind),
	local_chpsim_registry, 
"Given a random-access chpsim trace handle @var{tr}, return the dynamic event "
"occurring at index @var{ind}.") {
	size_t index;
	extract_scm(ind, index);	// will throw on error
	scm_assert_smob_type(raw_chpsim_trace_random_accessor_tag, tr);
	scm_chpsim_trace_random_accessor* const ptr =
		scm_smob_to_chpsim_trace_random_accessor_ptr(tr);
	if (!(ptr && ptr->good())) {
		scm_misc_error(FUNC_NAME,
			"CHPSIM trace random-access handle invalid.", SCM_EOL);
		return SCM_UNSPECIFIED;
	}
	const size_t max = ptr->num_entries();
	if (index >= max) {
		scm_misc_error(FUNC_NAME, 
			"Error: invalid event index.", SCM_EOL);
		return SCM_UNSPECIFIED;
	}
	const event_trace_point& tp((*ptr)[index]);
	return scm_from_event_trace_point(tp, index);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_ei globally allocated event index.  
	\return pair(index, event-smob).
 */
#define	FUNC_NAME "chpsim-get-event"
HAC_GUILE_DEFINE(wrap_chpsim_get_event, FUNC_NAME, 1, 0, 0, (SCM sei),
	local_chpsim_registry, 
"Looks up the raw-event entry indexed at @var{sei}, and returns a pair of "
"(index . event-smob).") {
	size_t ei;
	extract_scm(sei, ei);	// can throw error (returns good_bool)
	const size_t max = chpsim_state->event_pool_size();
	if (ei < max) {
		SCM ret_smob;
		SCM_NEWSMOB(ret_smob, raw_chpsim_event_node_ptr_tag,
			&chpsim_state->get_event(ei));
		return scm_cons(sei, ret_smob);	// pair
	} else {
		scm_misc_error(FUNC_NAME, 
			"Error: invalid event index.", SCM_EOL);
		// max is max
		return SCM_UNSPECIFIED;
	}
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the size of the chpsim-state's event pool (fixed).
 */
#define	FUNC_NAME "chpsim-num-events"
HAC_GUILE_DEFINE(wrap_chpsim_num_events, FUNC_NAME, 0, 0, 0, (void),
	local_chpsim_registry, 
"Returns the fixed size of chpsim\'s allocated event pool.") {
	return make_scm(chpsim_state->event_pool_size());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC
#undef	HAC_GUILE_DEFINE

//=============================================================================
BEGIN_C_DECLS
using namespace HAC::guile_wrap;
using util::guile::scm_gsubr_type;

/**
	Registers shared library functions for to guile interpreter.
	These functions are INDEPENDENT of the trace mechanism.
	TODO: namespace-ing exported functions?
 */
static
void
__libhackt_chpsim_guile_init(void* unused) {
	NEVER_NULL(chpsim_state);
#if 0
	libhackt_guile_init();		// prerequisite module
#else
//	scm_init_hackt_libhackt_primitives_module();
	scm_c_use_module("hackt hackt-primitives");
#endif
	// initialize any smob types we use
	raw_chpsim_event_node_ptr_smob_init();

	// native operations on chpsim-event SMOBs.
	import_chpsim_event_node_functions();
	util::for_all(local_chpsim_registry, util::caller());
#if HAVE_ATEXIT
	const int x = atexit(release_chpsim_wrap_resources_at_exit);
	INVARIANT(!x);
#endif
}	// end libhackt_chpsim_guile_init

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers shared library functions for to guile interpreter.
	These functions are DEPENDENT on the trace mechanism.
	TODO: namespace-ing exported functions?
 */
static
void
__libhackt_chpsim_trace_guile_init(void* unused) {
	NEVER_NULL(chpsim_state);
#if 0
	libhackt_guile_init();		// prerequisite module
#else
//	scm_init_hackt_libhackt_primitives_module();
	scm_c_use_module("hackt hackt-primitives");
	scm_c_use_module("hackt chpsim-primitives");
#endif
	// initialize any smob types we use
	raw_chpsim_trace_modes_smob_init();

	// native operations on chpsim-event SMOBs.
	import_hackt_chpsim_trace_stream_functions();
	// more trace-related functions
	// export all functions as part of module interface
	util::for_all(local_chpsim_trace_registry, util::caller());
}	// end libhackt_chpsim_guile_init

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Register interface function, wrapped call.  
 */
void
libhackt_chpsim_guile_init(void) {
	__libhackt_chpsim_guile_init(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register interface function, wrapped call.  
 */
void
libhackt_chpsim_trace_guile_init(void) {
	__libhackt_chpsim_trace_guile_init(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to load the functions into a module.  
 */
void
scm_init_hackt_chpsim_primitives_module(void) {
	scm_c_define_module("hackt chpsim-primitives",
		__libhackt_chpsim_guile_init, NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to load the functions into a module.  
 */
void
scm_init_hackt_chpsim_trace_primitives_module(void) {
	scm_c_define_module("hackt chpsim-trace-primitives",
		__libhackt_chpsim_trace_guile_init, NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
END_C_DECLS

