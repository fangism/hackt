/**
	\file "multi_pool_module_test.cc"
	Testing robust static initialization of separate memory pools.
	This is just a dummy file to provide a main symbol.
	$Id: multi_pool_module_test.cc,v 1.6 2006/02/26 02:28:04 fang Exp $
 */

#define	ENABLE_STACKTRACE		1
#define	ENABLE_STATIC_TRACE		1

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include "util/using_ostream.hh"

#include "pool_module_a.hh"
#include "pool_module_b.hh"

#include "util/memory/list_vector_pool.hh"
#include "util/stacktrace.hh"
#include "util/static_trace.hh"

STATIC_TRACE_BEGIN("test-main")

// not required because we have no static globals
// REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dum)
// REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(twiddle_dee)

static void
never_let_go(void) {
	STACKTRACE("never_let_go()");
	static excl_ptr<twiddle_dum> hold_on1(new twiddle_dum);
	static excl_ptr<twiddle_dee> hold_on2(new twiddle_dee);
}

int
main(int, char*[]) {
	STACKTRACE("MAIN()");	// magic string used for filtering for test
	cerr << "done with static initialization, start of main()" << endl;
#if 1
	never_let_go();
	never_let_go();
	never_let_go();
#endif
	cerr << "done with main(), begin static destruction." << endl;
	return 0;
}

STATIC_TRACE_END("test-main")

