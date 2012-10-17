/**
	\file "multikey_qmap_slice_test.cc"
	Testing the ability to detect dense sub-arrays.
	$Id: multikey_qmap_slice_test.cc,v 1.6 2006/02/26 02:28:05 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include "util/qmap.tcc"
#include "cube_slice.hh"
#include "plane_slice.hh"

using namespace std;
using namespace testing;

int
main(int, char*[]) {
	// 4x4 plane missing the 2 opposite corners
	plane_slice test0(0,4, 3,7, 5,9);
	test0.slice_test_1d();
	test0.slice_test_2d();

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

