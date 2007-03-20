/**
	\file "guile/chpsim-wrap.cc"
	$Id: chpsim-wrap.cc,v 1.2 2007/03/20 02:24:15 fang Exp $
 */

#include <iostream>
#include "guile/chpsim-wrap.h"
#include "guile/libhackt-wrap.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/Trace.h"
// #include "sim/chpsim/graph_options.h"
#include "util/stacktrace.h"
#include "util/libguile.h"
#include "util/guile_STL.h"

namespace HAC {
namespace guile_wrap {
using SIM::CHPSIM::State;
using SIM::CHPSIM::TraceManager;
// using SIM::CHPSIM::graph_options;
#include "util/using_ostream.h"
using util::guile::scm_assert_string;

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
}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================
BEGIN_C_DECLS
using namespace HAC::guile_wrap;
using util::guile::scm_gsubr_type;

/**
	Registers shared library functions for to guile interpreter.
 */
void
libhacktsim_guile_init(void) {
	NEVER_NULL(chpsim_state);
	libhackt_guile_init();		// prerequisite module
	// (use-modules (ice-9 streams))?
	// initialize any smob types we use
	scm_c_define_gsubr("dump-chpsim-struct", 0, 0, 0, 
		wrap_chpsim_dump_graph_alloc);
	scm_c_define_gsubr("dump-trace", 1, 0, 0, 
		reinterpret_cast<scm_gsubr_type>(wrap_chpsim_dump_trace));
#if HAVE_ATEXIT
	const int x = atexit(release_chpsim_wrap_resources_at_exit);
	INVARIANT(!x);
#endif
}	// end libhacktsim_guile_init

END_C_DECLS

