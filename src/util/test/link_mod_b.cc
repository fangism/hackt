/**
	\file "link_mod_b.cc"
	Testing ld linker characteristics.  
	$Id: link_mod_b.cc,v 1.2 2005/03/01 21:26:49 fang Exp $
 */

#define	ENABLE_STACKTRACE				1
#define	ENABLE_STATIC_TRACE				0
#define	USE_BOGUS					1

#include <iostream>
#include "using_ostream.h"

// #include "link_mod_a.h"
#include "link_mod_b.h"
#if USE_BOGUS
#include "bogus_stacktrace.h"
#else
#include "stacktrace.h"
#endif
#include "static_trace.h"

USING_STACKTRACE

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

