/**
	\file "packed_array_test.cc"
	$Id: packed_array_test.cc,v 1.5.36.1 2005/07/04 01:54:09 fang Exp $
 */

#include "packed_array.tcc"

using util::packed_array;
using util::packed_array_generic;
using util::multikey;
using util::multikey_generic;
using util::multikey_generator;

#include "using_ostream.h"
using std::inner_product;

typedef	multikey_generic<size_t>		generic_key_type;
typedef	packed_array<3, size_t, float>		float_3d;
typedef	packed_array<3, size_t, bool>		bool_3d;
typedef	packed_array_generic<size_t, int>	int_3d;

int
main(int argc, char* argv[]) {
{
	// testing simple construction of scalar using multikey
	// to specific (0) dimensions.  
	const generic_key_type empty;
	const int_3d a(empty);
	a.dump(cerr << "a = ") << endl;
	// the following should be equivalent to the above
	const generic_key_type empty2(0,0);
	const int_3d b(empty);
	b.dump(cerr << "b = ") << endl;
}
{
	float_3d foo;
	foo.dump(cerr << "foo: ") << endl;

	bool_3d boo;
	boo.dump(cerr << "boo: ") << endl;

	int_3d iky(3);
	iky.dump(cerr << "iky: ") << endl;
}

	float_3d::key_type dim;
	dim[0] = 3; dim[1] = 4; dim[2] = 5;

	float_3d barf(dim);
	bool_3d barb(dim);
	int_3d bari(dim);
	float_3d::key_type ind(dim);
{
	assert(!barf.range_check(dim));
	ind[0]--;
	assert(!barf.range_check(ind));
	ind[1]--;
	assert(!barf.range_check(ind));
	ind[2]--;
	assert(barf.range_check(ind));
	barf.dump(cerr << "barf: ") << endl;

	barf.resize(ind);
	assert(!barf.range_check(ind));
	barf.dump(cerr << "barf: ") << endl;

	ind[0]++; ind[1]++;
	barf.resize(ind);
	assert(!barf.range_check(ind));
	barf.dump(cerr << "barf: ") << endl;
	// should be 3 x 4 x 4 now
}
{
	ind = dim;
	assert(!barb.range_check(dim));
	ind[0]--;
	assert(!barb.range_check(ind));
	ind[1]--;
	assert(!barb.range_check(ind));
	ind[2]--;
	assert(barb.range_check(ind));
	barb.dump(cerr << "barb: ") << endl;

	barb.resize(ind);
	assert(!barb.range_check(ind));
	barb.dump(cerr << "barb: ") << endl;

	ind[0]++; ind[1]++;
	barb.resize(ind);
	assert(!barb.range_check(ind));
	barb.dump(cerr << "barb: ") << endl;
	// should be 3 x 4 x 4 now
}
{
	ind = dim;
	assert(!bari.range_check(dim));
	ind[0]--;
	assert(!bari.range_check(ind));
	ind[1]--;
	assert(!bari.range_check(ind));
	ind[2]--;
	assert(bari.range_check(ind));
	bari.dump(cerr << "bari: ") << endl;

	bari.resize(ind);
	assert(!bari.range_check(ind));
	bari.dump(cerr << "bari: ") << endl;

	ind[0]++; ind[1]++;
	bari.resize(ind);
	assert(!bari.range_check(ind));
	bari.dump(cerr << "bari: ") << endl;
	bari.dump_values(cerr << "bari: ") << endl;
	// should be 3 x 4 x 4 now
}

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
		barf[key_gen] = val;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	barf.dump(cerr << "barf: ") << endl;
}
{
	float_3d::key_generator_type key_gen;
	key_gen.upper_corner = ind -float_3d::ones;
	key_gen.lower_corner = float_3d::key_type();
	float_3d::key_type scale;
	scale[0] = 100; scale[1] = 10; scale[2] = 1;
	do {
		// inner product is actually defined for bools
		// use uses multiplication and addition
		const size_t val =
			inner_product(key_gen.begin(), key_gen.end(), 
			scale.begin(), 0);
		barb[key_gen] = val;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	barb.dump(cerr << "barb: ") << endl;
}
{
	float_3d::key_generator_type key_gen;
	key_gen.upper_corner = ind -float_3d::ones;
	key_gen.lower_corner = float_3d::key_type();
	float_3d::key_type scale;
	scale[0] = 100; scale[1] = 10; scale[2] = 1;
	do {
		// inner product is actually defined for bools
		// use uses multiplication and addition
		const size_t val =
			inner_product(key_gen.begin(), key_gen.end(), 
			scale.begin(), 0);
		bari[key_gen] = val;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	bari.dump(cerr << "bari: ") << endl;
}

	return 0;
}

