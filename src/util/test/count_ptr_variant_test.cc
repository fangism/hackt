/**
	\file "count_ptr_variant_test.cc"
	$Id: count_ptr_variant_test.cc,v 1.1 2005/12/10 03:57:00 fang Exp $
 */
// #include "util/memory/pointer_classes_fwd.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/count_array_ptr.tcc"
#include "util/memory/count_malloc_ptr.tcc"
// deliberately including multiple times to test include protection
#include "util/memory/count_array_ptr.h"
#include "util/memory/count_malloc_ptr.h"
#include "util/memory/count_ptr.h"
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
	const count_array_ptr<int> ii(new int[100]);

	// this will be freed upon destruction
	const count_malloc_ptr<int>
		im(static_cast<int*>(malloc(sizeof(int)*256)));
}
{
	const count_array_ptr<int> im(new int[666]);
	const count_array_ptr<int> jm(im);
	const count_array_ptr<int> km = im;
	INVARIANT(jm);
	INVARIANT(km);
}
{
	const count_malloc_ptr<int>
		im(static_cast<int*>(malloc(sizeof(int)*256)));
	// will steal ownership
	count_malloc_ptr<int> jm(im);
	count_malloc_ptr<int> km = jm;
	INVARIANT(jm);
	INVARIANT(km);
}
	return 0;
}


