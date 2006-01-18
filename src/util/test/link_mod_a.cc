/**
	\file "link_mod_a.cc"
	Testing ld linker characteristics.  
	$Id: link_mod_a.cc,v 1.3.22.1 2006/01/18 06:25:16 fang Exp $
 */

#define	ENABLE_STACKTRACE				1
#define	ENABLE_STATIC_TRACE				0
#define	USE_BOGUS					1

#include <iostream>
#include "util/using_ostream.h"

#include "link_mod_a.h"
// #include "link_mod_b.h"
#if USE_BOGUS
#include "bogus_stacktrace.h"
#else
#include "util/stacktrace.h"
#endif
#include "util/static_trace.h"

//-----------------------------------------------------------------------------
// specializations

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dum, "twiddle_dum")
}

//-----------------------------------------------------------------------------
// start of static initializers

STATIC_TRACE_BEGIN("twiddle_dum module")

// causes some ld to warn about multiple symbol definitions
#if USE_BOGUS
REQUIRES_BOGUS_STACKTRACE_STATIC_INIT
#else
REQUIRES_STACKTRACE_STATIC_INIT
#endif

twiddle_dum::twiddle_dum() {
	STACKTRACE("construct twiddle_dum");
//	cerr << "at " << this << endl;
}

twiddle_dum::~twiddle_dum() {
	STACKTRACE("destroy twiddle_dum");
//	cerr << "at " << this << endl;
}

STATIC_TRACE_END("twiddle_dum module")

// end of static initializers
//-----------------------------------------------------------------------------

