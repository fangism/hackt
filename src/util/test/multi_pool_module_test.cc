/**
	\file "multi_pool_module_test.cc"
	Testing robust static initialization of separate memory pools.
	This is just a dummy file to provide a main symbol.
	$Id: multi_pool_module_test.cc,v 1.3 2005/06/21 21:26:42 fang Exp $
 */

#define	ENABLE_STACKTRACE		1
#define	ENABLE_STATIC_TRACE		1

#include <iostream>
#include "using_ostream.h"

#include "pool_module_a.h"
#include "pool_module_b.h"

#include "memory/list_vector_pool.h"
#include "stacktrace.h"
#include "static_trace.h"

STATIC_TRACE_BEGIN("test-main")

USING_STACKTRACE

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
main(int argc, char* argv[]) {
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

