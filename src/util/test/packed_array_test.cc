/**
	\file "packed_array_test.cc"
	$Id: packed_array_test.cc,v 1.2 2004/12/16 03:50:59 fang Exp $
 */

#include "packed_array.tcc"

using util::packed_array;
#include "using_ostream.h"

typedef	packed_array<3, float>	float_3d;

int
main(int argc, char* argv[]) {
{
	float_3d foo;
	foo.dump(cerr) << endl;
}

	float_3d::key_type dim;
	dim[0] = 3; dim[1] = 4; dim[2] = 5;

	float_3d bar(dim);
	assert(!bar.range_check(dim));
	float_3d::key_type ind(dim);
	ind[0]--;
	assert(!bar.range_check(ind));
	ind[1]--;
	assert(!bar.range_check(ind));
	ind[2]--;
	assert(bar.range_check(ind));
	bar.dump(cerr) << endl;

	bar.resize(ind);
	assert(!bar.range_check(ind));
	bar.dump(cerr) << endl;

	return 0;
}

