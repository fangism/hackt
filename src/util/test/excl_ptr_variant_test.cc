/**
	\file "excl_ptr_variant_test.cc"
	$Id: excl_ptr_variant_test.cc,v 1.3 2006/02/26 02:28:04 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

// #include "util/memory/pointer_classes_fwd.hh"
#include "util/memory/excl_array_ptr.hh"
#include "util/memory/excl_ptr.hh"
#include "util/memory/excl_malloc_ptr.hh"
// deliberately including multiple times to test include protection
#include "util/memory/excl_array_ptr.hh"
#include "util/memory/excl_ptr.hh"
#include "util/memory/excl_malloc_ptr.hh"

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


