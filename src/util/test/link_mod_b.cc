/**
	\file "link_mod_b.cc"
	Testing ld linker characteristics.  
	$Id: link_mod_b.cc,v 1.4 2006/01/22 06:53:46 fang Exp $
 */

#define	ENABLE_STACKTRACE				1
#define	ENABLE_STATIC_TRACE				0
#define	USE_BOGUS					1

#include <iostream>
#include "util/using_ostream.hh"

// #include "link_mod_a.hh"
#include "link_mod_b.hh"
#if USE_BOGUS
#include "bogus_stacktrace.hh"
#else
#include "util/stacktrace.hh"
#endif
#include "util/static_trace.hh"

//-----------------------------------------------------------------------------
// specializations

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dee, "twiddle_dee")
}

//-----------------------------------------------------------------------------
// start of static initializers

STATIC_TRACE_BEGIN("twiddle_dee module")

// causes some ld to warn about multiple symbol definitions
#if USE_BOGUS
REQUIRES_BOGUS_STACKTRACE_STATIC_INIT
#else
REQUIRES_STACKTRACE_STATIC_INIT
#endif

twiddle_dee::twiddle_dee() {
	STACKTRACE("construct twiddle_dee");
//	cerr << "at " << this << endl;
}

twiddle_dee::~twiddle_dee() {
	STACKTRACE("destroy twiddle_dee");
//	cerr << "at " << this << endl;
}

STATIC_TRACE_END("twiddle_dee module")

// end of static initializers
//-----------------------------------------------------------------------------

