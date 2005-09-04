/**
	\file "multikey_qmap_slice_test.cc"
	Testing the ability to detect dense sub-arrays.
	$Id: multikey_qmap_slice_test.cc,v 1.4 2005/09/04 21:15:12 fang Exp $
 */

#include "util/qmap.tcc"
#include "cube_slice.h"

using namespace std;
using namespace testing;

int
main(int, char*[]) {
	// truncate upper corner
	cube_slice test1(10,20,30, 14,24,34, 60,68);
	test1.slice_test_1d();
	test1.slice_test_2d();
	test1.slice_test_3d();

	// truncate upper and lower corners
	cube_slice test2(0,10,20, 4,14,24, 32,40);
	test2.slice_test_1d();
	test2.slice_test_2d();
	test2.slice_test_3d();

	return 0;
}

