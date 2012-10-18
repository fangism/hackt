/**
	\file "count_ptr_variant_test.cc"
	$Id: count_ptr_variant_test.cc,v 1.3 2006/02/26 02:28:03 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

// #include "util/memory/pointer_classes_fwd.hh"
#include "util/memory/count_ptr.tcc"
#include "util/memory/count_array_ptr.tcc"
#include "util/memory/count_malloc_ptr.tcc"
// deliberately including multiple times to test include protection
#include "util/memory/count_array_ptr.hh"
#include "util/memory/count_malloc_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/memory/count_array_ptr.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/memory/count_malloc_ptr.tcc"

#include <cstdlib>
using util::memory::count_ptr;
using util::memory::count_array_ptr;
using util::memory::count_malloc_ptr;

int
main(int, char*[]) {
{
	// this will be deleted upon destruction
	const count_ptr<int> i(new int);

	// this will be deleted [] upon destruction
	const count_array_ptr<int>::type ii(new int[100]);

	// this will be freed upon destruction
	const count_malloc_ptr<int>::type
		im(static_cast<int*>(malloc(sizeof(int)*256)));
}
{
	const count_array_ptr<int>::type im(new int[666]);
	const count_array_ptr<int>::type jm(im);
	const count_array_ptr<int>::type km = im;
	INVARIANT(jm);
	INVARIANT(km);
}
{
	const count_malloc_ptr<int>::type
		im(static_cast<int*>(malloc(sizeof(int)*256)));
	// will steal ownership
	count_malloc_ptr<int>::type jm(im);
	count_malloc_ptr<int>::type km = jm;
	INVARIANT(jm);
	INVARIANT(km);
}
	return 0;
}


