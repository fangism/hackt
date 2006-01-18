/**
	\file "excl_ptr_variant_test.cc"
	$Id: excl_ptr_variant_test.cc,v 1.1.4.1 2006/01/18 06:25:16 fang Exp $
 */
// #include "util/memory/pointer_classes_fwd.h"
#include "util/memory/excl_array_ptr.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/excl_malloc_ptr.h"
// deliberately including multiple times to test include protection
#include "util/memory/excl_array_ptr.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/excl_malloc_ptr.h"

#include <cstdlib>
using util::memory::excl_ptr;
using util::memory::excl_array_ptr;
using util::memory::excl_malloc_ptr;

int
main(int, char*[]) {
{
	// this will be deleted upon destruction
	const excl_ptr<int> i(new int);

	// this will be deleted [] upon destruction
	const excl_array_ptr<int>::type ii(new int[100]);

	// this will be freed upon destruction
	const excl_malloc_ptr<int>::type
		im(static_cast<int*>(malloc(sizeof(int)*256)));
}
{
	excl_malloc_ptr<int>::type
		im(static_cast<int*>(malloc(sizeof(int)*256)));
	// will steal ownership
	excl_malloc_ptr<int>::type
		jm = im;
	INVARIANT(jm);
	MUST_BE_NULL(im);
}
	return 0;
}


