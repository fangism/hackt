/**
	'static_count_ptr_test.cc"
	Testing "unsafe" uses of count_ptr.  
	$Id: static_count_ptr_test.cc,v 1.1.2.1 2005/01/24 19:47:59 fang Exp $
 */

#define	ENABLE_STACKTRACE		1

#include <iostream>
#include "memory/count_ptr.h"
#include "memory/list_vector_pool.h"
#include "named_pooled_thing.h"
#include "stacktrace.h"
#include "using_ostream.h"

USING_STACKTRACE
using namespace util::memory;

typedef	count_ptr<named_thing>	ref_type;

// REQUIRES_STACKTRACE_STATIC_INIT
// required only if dynamically allocating during static initialization
// REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(named_thing)

ref_type
factory_func(void) {
	static named_thing* local_obj = new named_thing;
	static size_t* local_count = new size_t(0);
	return ref_type(local_obj, local_count);
}

int
main(int argc, char* argv[]) {
	STACKTRACE_VERBOSE;
#if 0
	static const ref_type static_ref = factory_func();

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

