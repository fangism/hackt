/**
	\file "discrete_interval_set_test.cc"
	Testing discrete_interval_set data structure.  
	$Id: discrete_interval_set_test.cc,v 1.5 2005/09/04 21:15:10 fang Exp $
 */

#include <iostream>
#include "util/discrete_interval_set.tcc"

#include "util/using_ostream.h"
using std::map;
using util::discrete_interval_set;

typedef	discrete_interval_set<long>		long_ranges;

static void map_test(void);
static void overlap_test(void);

int main(int, char*[]) {
if (0)	map_test();
if (0)	overlap_test();

	long_ranges r0;
	cout << r0 << endl;
	assert(static_cast<const long_ranges&>(r0).contains(66) == r0.end());
	assert(!r0.add_range(100,150));
	cout << r0 << endl;
	assert(static_cast<const long_ranges&>(r0).contains( 99) == r0.end());
	assert(static_cast<const long_ranges&>(r0).contains(100) != r0.end());
	assert(static_cast<const long_ranges&>(r0).contains(150) != r0.end());
	assert(static_cast<const long_ranges&>(r0).contains(151) == r0.end());


	assert(r0.add_range(100,140));
	cout << r0 << endl;

	assert(r0.add_range(110,140));
	cout << r0 << endl;

	assert(r0.add_range(110,150));
	cout << r0 << endl;

	assert(r0.add_range(100,150));
	cout << r0 << endl;
	cout << "should be {[100,150]} up to here." << endl;

	{	// extensions without overlap
		cout << "testing add_range with no overlap" << endl;
		long_ranges r1(r0);		// copy
		cout << r1 << endl;
		assert(!r1.add_range(60,70));
		cout << r1 << endl;
		assert(!r1.add_range(30,30));
		cout << r1 << endl;
		assert(!r1.add_range(20,25));
		cout << r1 << endl;
		assert(!r1.add_range(10,15));
		cout << r1 << endl;
		assert(!r1.add_range(80,90));
		cout << r1 << endl;
		assert(!r1.add_range(160,180));
		cout << r1 << endl;
	}
	{	// deletions without overlap
		cout << "testing delete_range with no overlap" << endl;
		long_ranges r1(r0);		// copy
		assert(!r1.delete_range(60,70));
		assert(!r1.delete_range(30,30));
		assert(!r1.delete_range(20,25));
		assert(!r1.delete_range(10,15));
		assert(!r1.delete_range(80,90));
		assert(!r1.delete_range(160,180));
		cout << r1 << endl;
	}

	{
		cout << "testing delete_range of tiny intervals" << endl;
		long_ranges r1;
		assert(!r1.add_range(10,10));
		assert(!r1.add_range(20,20));
		cout << r1 << endl;
		assert(!r1.delete_range(9,9));
		assert(!r1.delete_range(11,11));
		assert(!r1.delete_range(11,19));
		assert(!r1.delete_range(21,21));
		{
			long_ranges r2(r1);
			assert(r2.delete_range(10,10));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(9,10));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(10,11));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(9,11));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(20,20));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(19,20));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(20,21));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(19,21));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(10,20));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(9,20));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(10,21));
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(9,21));
		}
	}

	{	// extensions without overlap
		cout << "testing add_range with total overlap" << endl;
		long_ranges r1(r0);		// copy
		cout << r1 << endl;
		assert(r1.add_range(110,140));
		cout << r1 << endl;
		assert(!r1.add_range(0,50));
		cout << r1 << endl;
		assert(r1.add_range(0,0));
		cout << r1 << endl;
		assert(r1.add_range(0,10));
		cout << r1 << endl;
		assert(r1.add_range(40,50));
		cout << r1 << endl;
		assert(r1.add_range(100,120));
		cout << r1 << endl;
		assert(r1.add_range(130,150));
		cout << r1 << endl;
		assert(r1.add_range(125,135));
		cout << r1 << endl;
		assert(!r1.add_range(200,250));
		cout << r1 << endl;
		assert(r1.add_range(225,235));
		cout << r1 << endl;
		assert(r1.add_range(100,120));
		cout << r1 << endl;
		assert(r1.add_range(130,150));
		cout << r1 << endl;
		{
			cout << "testing delete_range with no overlap" << endl;
			assert(!r1.delete_range(-10,-1));
			assert(!r1.delete_range(-1,-1));
			assert(!r1.delete_range(51,99));
			assert(!r1.delete_range(151,199));
			assert(!r1.delete_range(251,299));
		}

		{
			cout << "checking contains()..." << endl;
			const long_ranges r2(r1);
			assert(r2.size() == 3);
			assert(r2.contains(-1) == r2.end());
			assert(r2.contains(0) != r2.end());
			assert(r2.contains(1) != r2.end());
			assert(r2.contains(49) != r2.end());
			assert(r2.contains(50) != r2.end());
			assert(r2.contains(51) == r2.end());
			assert(r2.contains(99) == r2.end());
			assert(r2.contains(100) != r2.end());
			assert(r2.contains(101) != r2.end());
			assert(r2.contains(149) != r2.end());
			assert(r2.contains(150) != r2.end());
			assert(r2.contains(151) == r2.end());
			assert(r2.contains(199) == r2.end());
			assert(r2.contains(200) != r2.end());
			assert(r2.contains(201) != r2.end());
			assert(r2.contains(249) != r2.end());
			assert(r2.contains(250) != r2.end());
			assert(r2.contains(251) == r2.end());
		}

		cout << "testing add_range with fusion" << endl;
		cout << r1 << endl;
		{
			long_ranges r2(r1);
			assert(!r2.add_range(-15,-1));	// should fuse
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(-15,10));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(50,60));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(51,60));	// should fuse
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(90,100));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(90,99));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(145,160));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(151,160));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(180,199));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(180,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(250,300));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(251,300));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}

		cout << "testing delete_range with partial-overlap" << endl;
		cout << r1 << endl;
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,0));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,10));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,49));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(1,49));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(11,40));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,50));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,55));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(45,50));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(45,55));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(1,50));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,50));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,60));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-10,60));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,99));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(51,100));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,100));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(40,110));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,100));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(99,100));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,101));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(99,101));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(150,150));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(150,155));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(145,150));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(145,155));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(101,150));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(101,149));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(125,125));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,150));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(90,150));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,160));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(90,160));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(40,160));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(1,160));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,160));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,100));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,101));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,149));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(199,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(151,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(150,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(101,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,200));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(51,200));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,200));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(25,200));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(1,200));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,200));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,200));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(200,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(200,201));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(199,200));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(199,201));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(150,201));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(101,201));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(100,201));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(51,201));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(50,201));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(45,201));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(1,201));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,201));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(250,250));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(250,255));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(245,250));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(245,255));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(201,250));
			cout << r2 << endl;
			assert(r2.size() == 3);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(200,250));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(201,249));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(222,223));
			cout << r2 << endl;
			assert(r2.size() == 4);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(2,223));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(200,260));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(190,260));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(140,260));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(101,260));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(99,260));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(49,260));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(0,260));
			cout << r2 << endl;
			assert(r2.size() == 0);
		}
		{
			long_ranges r2(r1);
			assert(r2.delete_range(-1,260));
			cout << r2 << endl;
			assert(r2.size() == 0);
		}
		

		cout << "testing fusing on both sides" << endl;
		{
			long_ranges r2(r1);
			assert(r2.add_range(50,100));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(50,99));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(51,100));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		{
			long_ranges r2(r1);
			assert(!r2.add_range(51,99));
			cout << r2 << endl;
			assert(r2.size() == 2);
		}
		cout << "testing fusing over interval sides" << endl;
		cout << r1 << endl;
		{
			long_ranges r2(r1);
			assert(r2.add_range(50,200));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(50,199));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(51,200));
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(51,199));	// does overlap!
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
		{
			long_ranges r2(r1);
			assert(r2.add_range(52,198));	// does not fuse
			cout << r2 << endl;
			assert(r2.size() == 3);
			assert(!r2.add_range(199,199));	// fuses
			cout << r2 << endl;
			assert(r2.size() == 2);
			assert(!r2.add_range(51,51));	// fuses
			cout << r2 << endl;
			assert(r2.size() == 1);
		}
	}

	cout << "testing add_range with partial overlap" << endl;
	cout << r0 << endl;
	{
		long_ranges r1(r0);		// copy
		assert(r1.add_range(90,150));
		cout << r1 << endl;
		assert(r1.add_range(80,100));
		cout << r1 << endl;
		assert(r1.add_range(70,160));
		cout << r1 << endl;
	}
	{
		long_ranges r1(r0);		// copy
		assert(r1.add_range(100,160));
		cout << r1 << endl;
		assert(r1.add_range(140,170));
		cout << r1 << endl;
		assert(r1.add_range(90,180));
		cout << r1 << endl;
	}
	cout << "testing add_range with one-sided overlap" << endl;
	cout << r0 << endl;
	{
		long_ranges r1(r0);		// copy
		assert(!r1.add_range(0,50));
		cout << r1 << endl;
		assert(r1.size() == 2);
		{
			{	// don't bother printing
				long_ranges r2(r1);
				assert(!r2.add_range(-20,-10));
				assert(r2.size() == 3);
			}
			{	// don't bother printing
				long_ranges r2(r1);
				assert(!r2.add_range(-20,-1));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,10));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,60));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,99));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,100));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,150));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(-20,200));
				assert(r2.size() == 1);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,10));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,60));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,99));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,100));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,150));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(0,200));
				assert(r2.size() == 1);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,10));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,60));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,99));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,100));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,150));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(1,200));
				assert(r2.size() == 1);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(r2.add_range(50,60));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(50,99));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(50,100));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(50,150));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(50,200));
				assert(r2.size() == 1);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(!r2.add_range(51,60));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(!r2.add_range(51,99));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(51,100));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(51,150));
				assert(r2.size() == 1);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(51,200));
				assert(r2.size() == 1);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(!r2.add_range(99,99));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(99,100));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(99,150));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(99,200));
				assert(r2.size() == 2);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(r2.add_range(100,100));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(100,150));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(100,200));
				assert(r2.size() == 2);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(r2.add_range(150,150));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(r2.add_range(150,200));
				assert(r2.size() == 2);
			}
		}
		{
			{
				long_ranges r2(r1);
				assert(!r2.add_range(151,151));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(!r2.add_range(151,200));
				assert(r2.size() == 2);
			}
			{
				long_ranges r2(r1);
				assert(!r2.add_range(152,200));
				assert(r2.size() == 3);
			}
		}
	}

	return 0;
}

/**
	Figuring out what lower_bound and upper_bound return.  
 */
void
map_test(void) {
	map<int,int>	foo;
	foo[100] = 150;
	foo[50] = 80;
	foo[200] = 256;
	cout << "foo[100] = 150;" << endl;
	cout << "foo[50] = 80;" << endl;
	cout << "foo[200] = 256;" << endl;

	{
	map<int,int>::const_iterator bar0 = foo.lower_bound(110);
	cout << "lower_bound(110):" << endl;
	if (bar0 != foo.end())
		cout << "bar0 = " << bar0->first << ", "
			<< bar0->second << endl;
	else	cout << "end." << endl;
	}
	{
	map<int,int>::const_iterator bar0b = foo.lower_bound(100);
	cout << "lower_bound(100):" << endl;
	if (bar0b != foo.end())
		cout << "bar0b = " << bar0b->first << ", "
			<< bar0b->second << endl;
	else	cout << "end." << endl;
	}
	{
	map<int,int>::const_iterator bar1 = foo.lower_bound(90);
	cout << "lower_bound(90):" << endl;
	if (bar1 != foo.end())
		cout << "bar1 = " << bar1->first << ", "
			<< bar1->second << endl;
	else	cout << "end." << endl;
	}
	{
	map<int,int>::const_iterator bar2 = foo.upper_bound(110);
	cout << "upper_bound(110):" << endl;
	if (bar2 != foo.end())
		cout << "bar2 = " << bar2->first << ", "
			<< bar2->second << endl;
	else	cout << "end." << endl;
	}
	{
	map<int,int>::const_iterator bar2b = foo.upper_bound(100);
	cout << "upper_bound(100):" << endl;
	if (bar2b != foo.end())
		cout << "bar2b = " << bar2b->first << ", "
			<< bar2b->second << endl;
	else	cout << "end." << endl;
	}
	{
	map<int,int>::const_iterator bar3 = foo.upper_bound(90);
	cout << "upper_bound(90):" << endl;
	if (bar3 != foo.end())
		cout << "bar3 = " << bar3->first << ", "
			<< bar3->second << endl;
	else	cout << "end." << endl;
	}
}

void overlap_test(void) {

}


