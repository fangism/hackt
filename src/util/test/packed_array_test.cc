/**
	\file "packed_array_test.cc"
	$Id: packed_array_test.cc,v 1.1 2004/12/15 23:31:18 fang Exp $
 */

#include "packed_array.tcc"

using util::packed_array;

typedef	packed_array<3, float>	float_3d;

int
main(int argc, char* argv[]) {
	float_3d foo;

	float_3d::key_type dim;
	dim[0] = 3; dim[1] = 4; dim[2] = 5;
	float_3d bar(dim);

	return 0;
}

