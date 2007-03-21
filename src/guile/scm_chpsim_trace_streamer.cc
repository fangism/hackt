/**
	\file "guile/scm_chpsim_trace_streamer.cc"
	$Id: scm_chpsim_trace_streamer.cc,v 1.1.2.2 2007/03/21 20:19:29 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "guile/scm_chpsim_trace_streamer.h"
// #include <iostream>
// #include <sstream>

namespace HAC {
namespace guile_wrap {
using HAC::SIM::CHPSIM::TraceManager;
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
		reinterpret_cast<scm_chpsim_trace_stream*>(
			SCM_SMOB_DATA(obj));
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

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

