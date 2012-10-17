/**
	\file "guile/chpsim-wrap.cc"
	$Id: chpsim-wrap.cc,v 1.10 2010/04/07 00:12:58 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "sim/chpsim/State.hh"
#include "sim/chpsim/TraceIterators.hh"
// #include "sim/chpsim/graph_options.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/module.hh"
#include "Object/global_channel_entry.hh"
#include "Object/nonmeta_channel_manipulator.hh"
#include "Object/nonmeta_variable.hh"
#include "Object/type/canonical_fundamental_chan_type.hh"
#include "Object/type/canonical_generic_datatype.hh"
#include "Object/def/built_in_datatype_def.hh"
#include "Object/def/enum_datatype_def.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/state_instance.hh"
#include "util/stacktrace.hh"
#include "util/libguile.hh"
#include "util/guile_STL.hh"
#include "guile/chpsim-wrap.hh"
#include "guile/libhackt-wrap.hh"
#include "guile/scm_chpsim_trace_streamer.hh"
#include "guile/scm_chpsim_event_node.hh"
#include "guile/hackt-documentation.hh"
#include "util/for_all.hh"
#include "util/caller.hh"

namespace HAC {
namespace guile_wrap {
using SIM::CHPSIM::State;
using SIM::CHPSIM::TraceManager;
using SIM::event_trace_point;
using SIM::CHPSIM::state_trace_window_base;
using SIM::CHPSIM::state_trace_time_window;
using entity::canonical_fundamental_chan_type_base;
using entity::canonical_generic_datatype;
using entity::datatype_definition_base;
using entity::built_in_datatype_def;
using entity::enum_datatype_def;
using entity::ChannelData;
using entity::channel_data_reader;
using entity::footprint;
using entity::class_traits;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
// using SIM::CHPSIM::graph_options;
#include "util/using_ostream.hh"
using util::memory::never_ptr;
using util::guile::scm_assert_string;
using util::guile::make_scm;
using util::guile::extract_scm;
using util::guile::scm_c_define_gsubr_exported;
USING_SCM_ASSERT_SMOB_TYPE
USING_SCM_FROM_LOCALE_SYMBOL

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
#define	FUNC_NAME "init-chpsim-state"
HAC_GUILE_DEFINE(wrap_init, FUNC_NAME, 0, 0, 0, (void),
	local_chpsim_registry, 
"Constructs the chpsim simulator state after loading object file.") {
	if (guile_wrap::obj_module) {
		if (!chpsim_state) {
			// first, cache all built-in channel types' summaries
			canonical_fundamental_chan_type_base::refresh_all_footprints();
			chpsim_state = count_ptr<State>(new State(*obj_module));
		}	// else silently ignore
	} else {
		scm_misc_error(FUNC_NAME,
		"Cannot construct simulator state until object file is loaded.", SCM_EOL);
	}
	return SCM_UNSPECIFIED;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "have-chpsim-state?"
HAC_GUILE_DEFINE(wrap_have_state, FUNC_NAME, 0, 0, 0, (void),
	local_chpsim_registry, 
"Query whether the chpsim simulator state has been initialized.") {
	return make_scm<bool>(chpsim_state);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
class changed_state_extractor_base {
protected:
	const footprint&		fp;

	explicit
	changed_state_extractor_base(const footprint& f) : fp(f) { }

};	// end struct changed_state_extractor_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convert a state_trace_point into SCM.  
 */
template <class Tag>
struct changed_state_extractor : protected changed_state_extractor_base {
	/**
		\param s the state_manager is unused and not needed.
	 */
	explicit
	changed_state_extractor(const footprint& s) :
		changed_state_extractor_base(s) { }

	SCM
	operator () (const typename state_trace_window_base<Tag>::iter_type::value_type& i) const {
		return scm_cons(scm_cons(
			scm_type_symbols[
				class_traits<Tag>::type_tag_enum_value],
			make_scm(i.global_index)),
			make_scm(i.raw_data));
			// specialize bool for #t and #f? too lazy...
	}
};	// end struct changed_state_extractor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef	HAVE_SCM_FROM_CHAR
/**
	Convert a state_trace_point into SCM.  
	Workaround specialization for bool data fields to avoid 
	SCM-char conversion (making output unreadable) (guile-1.6)
 */
template <>
struct changed_state_extractor<bool_tag> : protected changed_state_extractor_base {
	/**
		\param s the state_manager is unused and not needed.
	 */
	explicit
	changed_state_extractor(const footprint& s) :
		changed_state_extractor_base(s) { }

	SCM
	operator () (const state_trace_window_base<bool_tag>::iter_type::value_type& i) const {
		return scm_cons(scm_cons(
			scm_type_symbols[
				class_traits<bool_tag>::type_tag_enum_value],
			make_scm(i.global_index)),
			make_scm<int>(i.raw_data));
			// specialize bool for #t and #f? too lazy...
	}
};	// end struct changed_state_extractor<bool_tag>
#endif	// HAVE_SCM_FROM_CHAR

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to conver packed data into list of SCMs.  
 */
struct channel_data_scm_extractor {
	channel_data_reader			reader;

	explicit
	channel_data_scm_extractor(const ChannelData& d) : reader(d) { }

	/**
		Temporary 'dirty' implementation.  
		b/c don't feel like supporting an invasive virtual function.
		TODO: visitor?
		For reference, see "Object/nonmeta_channel_manipulator.cc".
	 */
	SCM
	operator () (const canonical_generic_datatype& d) {
		const never_ptr<const datatype_definition_base>
			b(d.get_base_def());
		const never_ptr<const built_in_datatype_def>
			bd(b.is_a<const built_in_datatype_def>());
		if (bd) {
			if (bd == &class_traits<bool_tag>::built_in_definition) {
#ifdef	HAVE_SCM_FROM_CHAR
				return make_scm
#else
			// for now, force construction through int type
			// in guile-1.6, going through char makes it unreadable
				return make_scm<int>
#endif
					(*reader.iter_ref<bool_tag>()++);
			} else if (bd == &class_traits<int_tag>::built_in_definition) {
				return make_scm(*reader.iter_ref<int_tag>()++);
			} else {
				cerr << "Unexpected built-in data type."
					<< endl;
			}
		} else {
			const never_ptr<const enum_datatype_def>
				ed(b.is_a<const enum_datatype_def>());
			if (ed) {
				return make_scm(*reader.iter_ref<enum_tag>()++);
			} else {
				cerr << "Unexpected built-in data type."
					<< endl;
			}
		}
		THROW_EXIT;
	}
};	// end struct channel_data_scm_extractor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	specialization for channel
	Needs module information to extract channels in correct tuple order.
 */
template <>
struct changed_state_extractor<channel_tag> :
		protected changed_state_extractor_base {
	explicit
	changed_state_extractor(const footprint& s) :
		changed_state_extractor_base(s) { }

	SCM
	operator () (const state_trace_window_base<channel_tag>::iter_type::value_type& i) const {
		STACKTRACE_VERBOSE;
		// TODO: construct structure of references, FINISH_ME
		static const SCM scm_ack = scm_permanent_object(
			scm_from_locale_symbol("ack"));	// 'ack symbol
		SCM scm_dat = scm_ack;
		STACKTRACE_INDENT_PRINT("index = " << i.global_index << endl);
		if (i.raw_data.has_trace_value()) {
			const canonical_fundamental_chan_type_base::datatype_list_type&
				cdl(fp.get_instance<channel_tag>(i.global_index -1)
					.channel_type->get_datatype_list());
				// get_instance is 0-indexed
			 // reverse! is destructive, and saves allocation
			scm_dat = scm_reverse_x(
				(*transform(cdl.begin(), cdl.end(),
					util::guile::scm_list_inserter(), 
					channel_data_scm_extractor(i.raw_data)))
				.list, SCM_LIST0);
		}
		return scm_cons(scm_cons(
			scm_type_symbols[
				class_traits<channel_tag>::type_tag_enum_value],
			make_scm(i.global_index)),
			scm_dat);
	}
};	// end struct changed_state_extractor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
static
SCM
__collect_changed_values(
		const state_trace_time_window::pseudo_const_iterator_range& r,
		const footprint& s) {
	const typename state_trace_window_base<Tag>::__pseudo_const_iterator_pair&
		p(r.template get<Tag>());
	
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("distance = " <<
		std::distance(p.first, p.second) << endl);
	// note: construction via forward iteration results in reverse list
	return (*transform(p.first, p.second, 
		util::guile::scm_list_inserter(),
		changed_state_extractor<Tag>(s))).list;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Converts current state-change entry into SCM object.
	Basically contains records of reference-value tuples.  
	\param i is the trace-event-index.
	\param tp is an iterator-paired slice of the state-changed
		variables for this event (and must be short-lived!)
 */
static
SCM
scm_from_state_trace_point(
		const TraceManager::state_change_streamer::
			pseudo_const_iterator_range& tp, 
		const size_t i, const footprint& s) {
	STACKTRACE_VERBOSE;
	return scm_cons(make_scm(i), 
		scm_cons(__collect_changed_values<bool_tag>(tp, s), 
		scm_cons(__collect_changed_values<int_tag>(tp, s),
		scm_cons(__collect_changed_values<enum_tag>(tp, s), 
		scm_cons(__collect_changed_values<channel_tag>(tp, s), 
			SCM_EOL)))));
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
	FUNC_NAME, 1, 0, 0, (SCM strm), 
	local_chpsim_trace_registry, 
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
	const SCM ret = scm_from_state_trace_point(tp, ptr->index(), 
		AS_A(const module&, *obj_module).get_footprint());
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
#if 0
	NEVER_NULL(chpsim_state);
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
#if 0
	NEVER_NULL(chpsim_state);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
END_C_DECLS

