/**
	\file "pool_module_b.cc"
	One module of a multimodule memory pool test.
	$Id: pool_module_b.cc,v 1.1.4.1.2.1 2005/01/24 19:47:58 fang Exp $
 */

#define	DEBUG_LIST_VECTOR_POOL				1
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		1
#define	ENABLE_STACKTRACE				1

#include <iostream>
#include "using_ostream.h"

#include "pool_module_b.h"
#include "pool_module_a.h"
#include "stacktrace.h"
#include "memory/pointer_classes.h"
#include "memory/list_vector_pool.h"

USING_STACKTRACE
using util::memory::count_ptr;
using util::memory::excl_ptr;


#if ENABLE_STACKTRACE
static const stacktrace __init_st__("start static init of module B.");
#else
static const ostream&
	__init_st__(cerr << "start static init of module B." << endl);
#endif

namespace util {
	SPECIALIZE_UTIL_WHAT_DEFINITION(twiddle_dee, "twiddle_dee")
} 

#if 0
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dum);
REQUIRES_UTIL_WHAT_STATIC_INIT(twiddle_dee);
#endif

REQUIRES_STACKTRACE_STATIC_INIT

REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dum);
LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(twiddle_dee, 8)

#if 1
static const excl_ptr<twiddle_dum>
my_twiddle_dum(new twiddle_dum);

static const count_ptr<twiddle_dum>
another_twiddle_dum(new twiddle_dum);
#endif

#if 1
static const excl_ptr<twiddle_dee>
my_twiddle_dee(new twiddle_dee);

static const count_ptr<twiddle_dee>
another_twiddle_dee(new twiddle_dee);
#endif

twiddle_dee::twiddle_dee() {
	STACKTRACE("construct twiddle_dee");
	cerr << "at " << this << endl;
}

twiddle_dee::~twiddle_dee() {
	STACKTRACE("destroy twiddle_dee");
	cerr << "at " << this << endl;
}

#if ENABLE_STACKTRACE
static const stacktrace __end_st__("end static init of module B.");
#else
static const ostream&
	__end_st__(cerr << "end static init of module B." << endl);
#endif

