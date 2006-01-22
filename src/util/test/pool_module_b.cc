/**
	\file "pool_module_b.cc"
	One module of a multimodule memory pool test.
	$Id: pool_module_b.cc,v 1.5 2006/01/22 06:53:47 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1
#define	ENABLE_STACKTRACE				1
#define	ENABLE_STATIC_TRACE				1

#include <iostream>
#include "util/using_ostream.h"

#include "pool_module_b.h"
#include "pool_module_a.h"
#include "util/stacktrace.h"
#include "util/memory/pointer_classes.h"
#include "util/memory/list_vector_pool.tcc"
#include "util/static_trace.h"

using util::memory::count_ptr;
using util::memory::excl_ptr;

//-----------------------------------------------------------------------------
// specializations

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dee, "twiddle_dee")
} 
namespace util {
namespace memory {
	// this changes the deletion policy to be lazy
	// which is safe for terminal (non-recursive) objects
	// this overrides the default eager destruction
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(twiddle_dee)
}
}

//-----------------------------------------------------------------------------
// start of static initializers

STATIC_TRACE_BEGIN("twiddle_dee module")

REQUIRES_STACKTRACE_STATIC_INIT

#if 0
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dum)
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dee)
#endif

REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dum)
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(twiddle_dee, 8)

static const excl_ptr<twiddle_dum>
my_twiddle_dum(new twiddle_dum);

static const count_ptr<twiddle_dum>
another_twiddle_dum(new twiddle_dum);

static const excl_ptr<twiddle_dee>
my_twiddle_dee(new twiddle_dee);

static const count_ptr<twiddle_dee>
another_twiddle_dee(new twiddle_dee);

twiddle_dee::twiddle_dee() {
	STACKTRACE("construct twiddle_dee");
	cerr << "at " << this << endl;
}

twiddle_dee::~twiddle_dee() {
	STACKTRACE("destroy twiddle_dee");
	cerr << "at " << this << endl;
}

STATIC_TRACE_END("twiddle_dee module")

// end of static initializers
//-----------------------------------------------------------------------------

