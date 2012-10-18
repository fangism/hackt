/**
	'static_count_ptr_test.cc"
	Testing "unsafe" uses of count_ptr.  
	$Id: static_count_ptr_test.cc,v 1.7 2006/04/27 00:17:58 fang Exp $
 */

#define	ENABLE_STACKTRACE		1

#define	ENABLE_STATIC_TRACE		0

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include "util/static_trace.hh"
STATIC_TRACE_BEGIN("static_count_ptr_test::main")

#include <iostream>
#include "util/memory/count_ptr.hh"
#include "util/memory/list_vector_pool.hh"
#include "named_pooled_thing.hh"
#include "util/stacktrace.hh"
#include "util/using_ostream.hh"
#include "util/attributes.h"

using namespace util::memory;

typedef	count_ptr<named_thing>	ref_type;

// REQUIRES_STACKTRACE_STATIC_INIT
// required only if dynamically allocating during static initialization
// REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(named_thing)

ref_type
factory_func(void) {
	static named_thing* local_obj = new named_thing;
	STATIC_RC_POOL_REF_INIT;
	static size_t* local_count = NEW_SIZE_T;
	static const size_t zero __ATTRIBUTE_UNUSED__ = (*local_count = 0);
	return ref_type(local_obj, local_count);
}

int
main(int, char*[]) {
	STACKTRACE_VERBOSE;
#if 1
	static const ref_type static_ref __ATTRIBUTE_UNUSED__ = factory_func();

	const ref_type local_ref = factory_func();

	int i = 0;
	for ( ; i < 4; i++) {
		STACKTRACE("for-loop");
		const ref_type for_ref = factory_func();
		cout << "count = " << for_ref.refs() << endl;
	}

	cout << "count = " << local_ref.refs() << endl;
#endif
	return 0;
}

STATIC_TRACE_END("static_count_ptr_test::main")

