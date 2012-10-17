/**
	\file "pool_module_a.cc"
	One module of a multimodule memory pool test.
	$Id: pool_module_a.cc,v 1.5 2006/01/22 06:53:46 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1
#define	ENABLE_STACKTRACE				1
#define	ENABLE_STATIC_TRACE				1

#include <iostream>
#include "util/using_ostream.hh"

#include "pool_module_a.hh"
#include "pool_module_b.hh"
#include "util/stacktrace.hh"
#include "util/memory/pointer_classes.hh"
#include "util/memory/list_vector_pool.tcc"
#include "util/static_trace.hh"

using util::memory::count_ptr;
using util::memory::excl_ptr;

//-----------------------------------------------------------------------------
// specializations

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dum, "twiddle_dum")
}
namespace util {
namespace memory {
	// this changes the deletion policy to be lazy
	// which is safe for terminal (non-recursive) objects
	// this overrides the default eager destruction
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(twiddle_dum)
}
}

//-----------------------------------------------------------------------------
// start of static initializers

STATIC_TRACE_BEGIN("twiddle_dum module")

REQUIRES_STACKTRACE_STATIC_INIT

#if 0
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dee)
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dum)
#endif

REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dee)
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(twiddle_dum, 8)

static const excl_ptr<twiddle_dee>
my_twiddle_dee(new twiddle_dee);

static const count_ptr<twiddle_dee>
another_twiddle_dee(new twiddle_dee);

static const excl_ptr<twiddle_dum>
my_twiddle_dum(new twiddle_dum);

static const count_ptr<twiddle_dum>
another_twiddle_dum(new twiddle_dum);

twiddle_dum::twiddle_dum() {
	STACKTRACE("construct twiddle_dum");
	cerr << "at " << this << endl;
}

twiddle_dum::~twiddle_dum() {
	STACKTRACE("destroy twiddle_dum");
	cerr << "at " << this << endl;
}

STATIC_TRACE_END("twiddle_dum module")

// end of static initializers
//-----------------------------------------------------------------------------

