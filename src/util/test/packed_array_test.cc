/**
	\file "packed_array_test.cc"
	$Id: packed_array_test.cc,v 1.3 2004/12/19 07:51:12 fang Exp $
 */

#include "packed_array.tcc"

using util::packed_array;
using util::multikey;
using util::multikey_generator;

#include "using_ostream.h"
using std::inner_product;

typedef	packed_array<3, float>	float_3d;
typedef	packed_array<3, bool>	bool_3d;

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

	ind[0]++; ind[1]++;
	bar.resize(ind);
	assert(!bar.range_check(ind));
	bar.dump(cerr) << endl;
	// should be 3 x 4 x 4 now

{
	float_3d::key_generator_type key_gen;
	key_gen.upper_corner = ind -float_3d::ones;
	key_gen.lower_corner = float_3d::key_type();
	float_3d::key_type scale;
	scale[0] = 100; scale[1] = 10; scale[2] = 1;
	do {
		const size_t val =
			inner_product(key_gen.begin(), key_gen.end(), 
			scale.begin(), 0);
		bar[key_gen] = val;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	bar.dump(cerr) << endl;
}

	return 0;
}

