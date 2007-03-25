/**
	\file "guile/scm_chpsim_trace_streamer.cc"
	$Id: scm_chpsim_trace_streamer.cc,v 1.1.2.3 2007/03/25 02:25:38 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "guile/scm_chpsim_trace_streamer.h"
#include <iostream>
// #include <sstream>
#include "util/guile_STL.h"

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
	scm_puts("<raw-chpsim-trace-stream>", port);
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
// primitive functions for trace-stream SMOBS


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
		// scm_error_misc()?
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
	Predicate.
	\return true if the trace-stream is still valid.  
 */
static
SCM
wrap_chpsim_trace_stream_valid_p(SCM s_tr) {
	STACKTRACE_VERBOSE;
#define	FUNC_NAME "chpsim-trace-valid?"
	const scm_chpsim_trace_stream* const ptr =
		scm_smob_to_chpsim_trace_stream_ptr(s_tr);
	return make_scm<bool>(ptr && ptr->good());
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Export primitive functions in current module.
	\pre chpsim-trace-stream smob type must already be registered.  
 */
void
import_hackt_chpsim_trace_stream_functions(void) {
	INVARIANT(raw_chpsim_trace_stream_tag);
	scm_c_define_gsubr_exported("dump-trace", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_dump_trace));
	scm_c_define_gsubr_exported("open-chpsim-trace", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_open_chpsim_trace));
	scm_c_define_gsubr_exported("chpsim-trace-valid?", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_trace_stream_valid_p));
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

