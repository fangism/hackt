/**
	\file "pool_module_a.cc"
	One module of a multimodule memory pool test.
	$Id: pool_module_a.cc,v 1.1.4.1.2.3 2005/01/25 05:25:07 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1
#define	ENABLE_STACKTRACE				1

#include <iostream>
#include "using_ostream.h"

#include "pool_module_a.h"
#include "pool_module_b.h"
#include "stacktrace.h"
#include "memory/pointer_classes.h"
#include "memory/list_vector_pool.h"
#include "static_trace.h"

USING_STACKTRACE
using util::memory::count_ptr;
using util::memory::excl_ptr;

STATIC_TRACE_BEGIN("twiddle_dum module")

REQUIRES_STACKTRACE_STATIC_INIT

#if 0
#if ENABLE_STACKTRACE
static const stacktrace __init_st__("start static init of module twiddle_dum.");
#else
static const ostream&
	__init_st__(cerr << "start static init of module twiddle_dum." << endl);
#endif
#endif

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dum, "twiddle_dum")
}

#if 0
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dee);
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dum);
#endif

REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dee);
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(twiddle_dum, 8)

#if 1
static const excl_ptr<twiddle_dee>
my_twiddle_dee(new twiddle_dee);

static const count_ptr<twiddle_dee>
another_twiddle_dee(new twiddle_dee);
#endif

#if 1
static const excl_ptr<twiddle_dum>
my_twiddle_dum(new twiddle_dum);

static const count_ptr<twiddle_dum>
another_twiddle_dum(new twiddle_dum);
#endif

twiddle_dum::twiddle_dum() {
	STACKTRACE("construct twiddle_dum");
	cerr << "at " << this << endl;
}

twiddle_dum::~twiddle_dum() {
	STACKTRACE("destroy twiddle_dum");
	cerr << "at " << this << endl;
}

#if 0
#if ENABLE_STACKTRACE
static const stacktrace __end_st__("end static init of module twiddle_dum.");
#else
static const ostream&
	__end_st__(cerr << "end static init of module twiddle_dum." << endl);
#endif
#endif

STATIC_TRACE_END("twiddle_dum module")

