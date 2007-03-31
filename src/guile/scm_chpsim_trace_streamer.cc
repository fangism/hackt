/**
	\file "guile/scm_chpsim_trace_streamer.cc"
	$Id: scm_chpsim_trace_streamer.cc,v 1.1.2.8 2007/03/31 04:40:17 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "guile/scm_chpsim_trace_streamer.h"
#include <iostream>
// #include <sstream>
#include "util/guile_STL.h"
#include "guile/hackt-documentation.h"
#include "util/for_all.h"
#include "util/caller.h"

namespace HAC {
namespace guile_wrap {
#include "util/using_ostream.h"
using HAC::SIM::CHPSIM::TraceManager;
using util::guile::scm_assert_string;
using util::guile::make_scm;
using util::guile::extract_scm;
using util::guile::scm_gsubr_type;
using util::guile::scm_c_define_gsubr_exported;

//=============================================================================
// chpsim-trace forward stream SMOB

/**
	Writeable private tag.  Write once-only please!
 */
static
scm_t_bits __raw_chpsim_trace_stream_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public read-only reference/alias.
 */
const
scm_t_bits& raw_chpsim_trace_stream_tag(__raw_chpsim_trace_stream_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This pattern is repetitive, define as template-bound function
	using util::memory::deallocation_policy?
	\return 0 always
 */
static
size_t
free_raw_chpsim_trace_stream(SCM obj) {
	TraceManager::entry_streamer* ptr =
		scm_smob_to_chpsim_trace_stream_ptr(obj);
	if (ptr) {
		delete ptr;
		ptr = NULL;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return non-zero to indicate success.
 */
static
int
print_raw_chpsim_trace_stream(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-chpsim-trace-stream>", port);
	// TODO: print something about state?
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
raw_chpsim_trace_stream_smob_init(void) {
if (!raw_chpsim_trace_stream_tag) {
	__raw_chpsim_trace_stream_tag = 
		scm_make_smob_type("raw-chpsim-trace-stream",
			sizeof(scm_chpsim_trace_stream));
	INVARIANT(raw_chpsim_trace_stream_tag);
//	scm_set_smob_mark(raw_chpsim_trace_stream_tag, ...);
	scm_set_smob_free(raw_chpsim_trace_stream_tag,
		free_raw_chpsim_trace_stream);
	scm_set_smob_print(raw_chpsim_trace_stream_tag,
		print_raw_chpsim_trace_stream);
//	scm_set_smob_equalp(raw_chpsim_trace_stream_tag, ...);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pointer to trace stream.  
	\throw C exception if type-assert fails.  
 */
scm_chpsim_trace_stream*
scm_smob_to_chpsim_trace_stream_ptr(const SCM& obj) {
	scm_assert_smob_type(raw_chpsim_trace_stream_tag, obj);
	return reinterpret_cast<scm_chpsim_trace_stream*>(SCM_SMOB_DATA(obj));
}

//=============================================================================
// chpsim-trace reverse stream SMOB

/**
	Writeable private tag.  Write once-only please!
 */
static
scm_t_bits __raw_chpsim_trace_reverse_stream_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public read-only reference/alias.
 */
const
scm_t_bits&
raw_chpsim_trace_reverse_stream_tag(__raw_chpsim_trace_reverse_stream_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This pattern is repetitive, define as template-bound function
	using util::memory::deallocation_policy?
	\return 0 always
 */
static
size_t
free_raw_chpsim_trace_reverse_stream(SCM obj) {
	TraceManager::entry_reverse_streamer* ptr =
		scm_smob_to_chpsim_trace_reverse_stream_ptr(obj);
	if (ptr) {
		delete ptr;
		ptr = NULL;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return non-zero to indicate success.
 */
static
int
print_raw_chpsim_trace_reverse_stream(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-chpsim-trace-reverse-stream>", port);
	// TODO: print something about state?
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
raw_chpsim_trace_reverse_stream_smob_init(void) {
if (!raw_chpsim_trace_reverse_stream_tag) {
	__raw_chpsim_trace_reverse_stream_tag = 
		scm_make_smob_type("raw-chpsim-trace-reverse-stream",
			sizeof(scm_chpsim_trace_reverse_stream));
	INVARIANT(raw_chpsim_trace_reverse_stream_tag);
//	scm_set_smob_mark(raw_chpsim_trace_reverse_stream_tag, ...);
	scm_set_smob_free(raw_chpsim_trace_reverse_stream_tag,
		free_raw_chpsim_trace_reverse_stream);
	scm_set_smob_print(raw_chpsim_trace_reverse_stream_tag,
		print_raw_chpsim_trace_reverse_stream);
//	scm_set_smob_equalp(raw_chpsim_trace_reverse_stream_tag, ...);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pointer to trace stream.  
	\throw C exception if type-assert fails.  
 */
scm_chpsim_trace_reverse_stream*
scm_smob_to_chpsim_trace_reverse_stream_ptr(const SCM& obj) {
	scm_assert_smob_type(raw_chpsim_trace_reverse_stream_tag, obj);
	return reinterpret_cast<scm_chpsim_trace_reverse_stream*>(
		SCM_SMOB_DATA(obj));
}

//=============================================================================
// chpsim-trace random accessor SMOB

/**
	Writeable private tag.  Write once-only please!
 */
static
scm_t_bits __raw_chpsim_trace_random_accessor_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public read-only reference/alias.
 */
const
scm_t_bits&
raw_chpsim_trace_random_accessor_tag(__raw_chpsim_trace_random_accessor_tag);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This pattern is repetitive, define as template-bound function
	using util::memory::deallocation_policy?
	\return 0 always
 */
static
size_t
free_raw_chpsim_trace_random_accessor(SCM obj) {
	TraceManager::random_accessor* ptr =
		scm_smob_to_chpsim_trace_random_accessor_ptr(obj);
	if (ptr) {
		delete ptr;
		ptr = NULL;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return non-zero to indicate success.
 */
static
int
print_raw_chpsim_trace_random_accessor(SCM obj, SCM port, scm_print_state* p) {
	scm_puts("#<raw-chpsim-trace-random-accessor>", port);
	// TODO: print something about state?
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
raw_chpsim_trace_random_accessor_smob_init(void) {
if (!raw_chpsim_trace_random_accessor_tag) {
	__raw_chpsim_trace_random_accessor_tag = 
		scm_make_smob_type("raw-chpsim-trace-random-accessor",
			sizeof(scm_chpsim_trace_random_accessor));
	INVARIANT(raw_chpsim_trace_random_accessor_tag);
//	scm_set_smob_mark(raw_chpsim_trace_random_accessor_tag, ...);
	scm_set_smob_free(raw_chpsim_trace_random_accessor_tag,
		free_raw_chpsim_trace_random_accessor);
	scm_set_smob_print(raw_chpsim_trace_random_accessor_tag,
		print_raw_chpsim_trace_random_accessor);
//	scm_set_smob_equalp(raw_chpsim_trace_random_accessor_tag, ...);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pointer to trace stream.  
	\throw C exception if type-assert fails.  
 */
scm_chpsim_trace_random_accessor*
scm_smob_to_chpsim_trace_random_accessor_ptr(const SCM& obj) {
	scm_assert_smob_type(raw_chpsim_trace_random_accessor_tag, obj);
	return reinterpret_cast<scm_chpsim_trace_random_accessor*>(
		SCM_SMOB_DATA(obj));
}

//-----------------------------------------------------------------------------
/**
	Register all smob types defined in this translation unit.
 */
void
raw_chpsim_trace_modes_smob_init(void) {
	raw_chpsim_trace_stream_smob_init();
	raw_chpsim_trace_reverse_stream_smob_init();
	raw_chpsim_trace_random_accessor_smob_init();
}

//=============================================================================
// primitive functions for trace-stream SMOBS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// local initialization registry
static
std::vector<scm_init_func_type>		local_registry;

#define HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, DOCSTRING) \
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT,			\
	VAR, ARGLIST, local_registry, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints text dump of chpsim trace fo stdout.
	\param s_str the name of trace file.
 */
#define	FUNC_NAME "dump-trace"
HAC_GUILE_DEFINE(wrap_chpsim_dump_trace, FUNC_NAME, 1, 0, 0, (SCM s_str),
"Produces a textual dump of a trace-file''s contents to stdout.") {
	scm_assert_string(s_str, FUNC_NAME, 1);
	const char* tf = scm_to_locale_string(s_str);
	if (TraceManager::text_dump(tf, cout)) {
		// scm_error_misc()?
		cerr << "Error opening trace file: " << tf << endl;
	}
	return SCM_UNSPECIFIED;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_str the name of the tracefile (string)
	\return smob of the newly opened tracefile.
 */
#define	FUNC_NAME "open-chpsim-trace"
HAC_GUILE_DEFINE(wrap_open_chpsim_trace, FUNC_NAME, 1, 0, 0, (SCM trfn),
"Opens a trace file named @var{trfn} for forward reading as a stream-like "
"object.  The return object is a smob representing the trace as a stream.") {
	STACKTRACE_VERBOSE;
	const std::string peek(scm_to_locale_string(trfn));	// 1.8
	// alternately string_to_locale_stringbuf
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<scm_chpsim_trace_stream>
		tf(new scm_chpsim_trace_stream(peek));
//	NEVER_NULL(tf);		// damn it, no operator bool in auto_ptr...
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_chpsim_trace_stream_tag, tf.release());
	return ret_smob;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_str the name of the tracefile (string)
	\return smob of the newly opened tracefile, in reverse.
 */
#define	FUNC_NAME "open-chpsim-trace-reverse"
HAC_GUILE_DEFINE(wrap_open_chpsim_reverse_trace, FUNC_NAME, 1, 0, 0, (SCM trfn),
"Opens a trace file named @var{trfn} for backward reading as a stream-like "
"object.  The return object is a smob representing the trace as a stream.") {
	STACKTRACE_VERBOSE;
	const std::string peek(scm_to_locale_string(trfn));	// 1.8
	// alternately string_to_locale_stringbuf
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<scm_chpsim_trace_reverse_stream>
		tf(new scm_chpsim_trace_reverse_stream(peek));
//	NEVER_NULL(tf);
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_chpsim_trace_reverse_stream_tag,
		tf.release());
	return ret_smob;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses a temporary reverse stream because it is already opened to the
	last epoch.  
 */
#define	FUNC_NAME "chpsim-trace-num-entries"
HAC_GUILE_DEFINE(wrap_chpsim_trace_num_entries, FUNC_NAME, 1, 0, 0, (SCM trf),
"Returns the number of trace event entries in trace file @var{trf}.") {
	const std::string peek(scm_to_locale_string(trf));	// 1.8
	const std::auto_ptr<scm_chpsim_trace_reverse_stream>
		tf(new scm_chpsim_trace_reverse_stream(peek));
//	NEVER_NULL(tf);
	return make_scm(tf->num_entries());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s_str the name of the tracefile (string)
	\return smob of the newly opened tracefile, for random access.
 */
#define	FUNC_NAME "open-chpsim-trace-accessor"
HAC_GUILE_DEFINE(wrap_open_chpsim_random_accessor, FUNC_NAME, 1, 0, 0, 
	(SCM trfn),
"Opens a trace file named @var{trfn} for random event index access.") {
	STACKTRACE_VERBOSE;
	const std::string peek(scm_to_locale_string(trfn));	// 1.8
	// alternately string_to_locale_stringbuf
	// alert: heap-allocating though naked pointer, copy-constructing
	std::auto_ptr<scm_chpsim_trace_random_accessor>
		tf(new scm_chpsim_trace_random_accessor(peek));
	SCM ret_smob;
	SCM_NEWSMOB(ret_smob, raw_chpsim_trace_random_accessor_tag,
		tf.release());
	return ret_smob;
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if object is a trace-stream.
 */
#define	FUNC_NAME "chpsim-trace?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_stream_p, FUNC_NAME, 1, 0, 0, 
		(SCM obj),
"Is @var{obj} a chpsim trace file (forward)?") {
	return make_scm<bool>(
		SCM_SMOB_PREDICATE(raw_chpsim_trace_stream_tag, obj));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if object is a reverse trace-stream.
 */
#define	FUNC_NAME "chpsim-trace-reverse?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_reverse_stream_p, FUNC_NAME, 1, 0, 0, 
		(SCM obj),
"Is @var{obj} a chpsim trace file (reverse)?") {
	return make_scm<bool>(
		SCM_SMOB_PREDICATE(raw_chpsim_trace_reverse_stream_tag, obj));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if object is a trace random-accessor.
 */
#define	FUNC_NAME "chpsim-trace-accessor?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_random_accessor_p, FUNC_NAME, 1, 0, 0, 
		(SCM obj),
"Is @var{obj} a chpsim trace file opened in random-access mode?") {
	return make_scm<bool>(
		SCM_SMOB_PREDICATE(raw_chpsim_trace_random_accessor_tag, obj));
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if the trace-stream is still valid.  
 */
#define	FUNC_NAME "chpsim-trace-valid?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_stream_valid_p, FUNC_NAME, 1, 0, 0, 
		(SCM trfs),
"Is the trace stream @var{trfs} still valid (entries remain)?") {
	STACKTRACE_VERBOSE;
	const scm_chpsim_trace_stream* const ptr =
		scm_smob_to_chpsim_trace_stream_ptr(trfs);
	return make_scm<bool>(ptr && ptr->good());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Predicate.
	\return true if the reverse trace-stream is still valid.  
 */
#define	FUNC_NAME "chpsim-trace-reverse-valid?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_reverse_stream_valid_p, FUNC_NAME, 1, 0, 0, 
		(SCM trfs),
"Is the reverse trace stream @var{trfs} still valid (entries remain)?") {
	STACKTRACE_VERBOSE;
	const scm_chpsim_trace_reverse_stream* const ptr =
		scm_smob_to_chpsim_trace_reverse_stream_ptr(trfs);
	return make_scm<bool>(ptr && ptr->good());
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Predicate.
	\return true if the trace-stream is still valid (always).  
 */
#define	FUNC_NAME "chpsim-trace-accessor-valid?"
HAC_GUILE_DEFINE(wrap_chpsim_trace_random_accessor_valid_p, FUNC_NAME, 1, 0, 0, 
		(SCM trfs),
"Is the trace accessor @var{trfs} still valid?") {
	STACKTRACE_VERBOSE;
	const scm_chpsim_trace_random_accessor* const ptr =
		scm_smob_to_chpsim_trace_random_accessor_ptr(trfs);
	return make_scm<bool>(ptr && ptr->good());	// not defined
}
#undef	FUNC_NAME
#endif

#undef	HAC_GUILE_DEFINE
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Export primitive functions in current module.
	Covers both forward and reverse streams.
	\pre chpsim-trace-stream smob type must already be registered.  
 */
void
import_hackt_chpsim_trace_stream_functions(void) {
	INVARIANT(raw_chpsim_trace_stream_tag);
	util::for_all(local_registry, util::caller());
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

