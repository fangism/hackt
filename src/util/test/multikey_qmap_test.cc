/**
	\file "multikey_qmap_test.cc"
	Test for multidimensional, queryable map with multidimensional keys.  
	Copied from "multidimensional_qmap_test.cc" with a few lines changes
	$Id: multikey_qmap_test.cc,v 1.6 2004/12/15 23:31:18 fang Exp $
 */

#include <iostream>
#include <string>
#include "sstream.h"
#include "qmap.tcc"
#include "multikey_qmap.tcc"
#include "STL/list.tcc"

USING_LIST
using std::string;
#include "using_ostream.h"
using std::ostringstream;
using namespace MULTIKEY_MAP_NAMESPACE;

#if 0
typedef	multikey_qmap<1, int, string>	test_map1d_type;
typedef	multikey_qmap<2, int, string>	test_map2d_type;
typedef	multikey_qmap<3, int, string>	test_map3d_type;
#else
typedef	multikey_map<1, int, string, qmap>	test_map1d_type;
typedef	multikey_map<2, int, string, qmap>	test_map2d_type;
typedef	multikey_map<3, int, string, qmap>	test_map3d_type;
#endif

int
main(int argc, char* argv[]) {
	test_map3d_type	str_map;
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	test_map3d_type::key_type ind;

	// or reference
	int* first = &ind[0];
	int* second = &ind[1];
	int* third = &ind[2];

	str_map[ind] = "Hello.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << str_map[ind] << endl;
	cout << "Map's population = " << str_map.population() << endl;

	str_map[ind] = "Goodbye.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << str_map[ind] << endl;
	cout << "Map's population = " << str_map.population() << endl;

	*third = 1;
	cout << "Nothing should follow this colon on this line: "
		<< str_map[ind] << endl << endl;

	cout << "The next dump should contain one blank entry.";
	str_map.dump(cout << endl << "Map: " << endl) << endl;
	cout << "Map's population = " << str_map.population() << endl;

	(*third)++;
	static_cast<const test_map3d_type&>(str_map)[ind];
				// prevents blank
	cout << "The next dump should still contain one blank entry.";
	str_map.dump(cout << endl << "Map: " << endl) << endl;
	cout << "Map's population = " << str_map.population() << endl;

#if 0
	string foo = str_map[ind];
	cout << "The next dump should still contain one blank entry.";
	str_map.dump(cout << endl << "Map: " << endl) << endl;
	cout << "Map's population = " << str_map.population() << endl;
#endif

	str_map[ind];		// creates a blank
	cout << "The next dump should contain two blank entries.";
	str_map.dump(cout << endl << "Map: " << endl) << endl;
	cout << "Map's population = " << str_map.population() << endl;

	// next create a ton of blanks
	for (*first = 2; *first < 4; (*first)++)
		for (*second = 2; *second < 4; (*second)++)
			for (*third = 2; *third < 4; (*third)++)
				str_map[ind];
	cout << "The next dump should contain many blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

#if 0
	// not a true multidimensional array, can't erase subdimensions
	test_map3d_type::index_arg_type eraser = make_iter_range(ind);
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain same blank entries as before.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	*first = *second = *third = 3;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain one fewer blank entry.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	eraser.second--;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	*second = 2;
	str_map.erase(eraser);
	cout << "The next dump should contain two fewer blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	eraser.second--;
	*first = 2;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain four fewer blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	eraser.second++;		// can't do ++++;
	eraser.second++;
	cout << "Back to " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	*first = 0; *second = 0; *third = 1;
	str_map.erase(eraser);
	cout << "The next dump should contain one fewer blank entry.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;
#endif

	str_map.clean();
	cout << "The next dump should contain no blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	for (*first = 0; *first < 4; (*first)++)
		for (*second = 0; *second < 4; (*second)++)
			for (*third = 0; *third < 4; (*third)++) {
				const int sum = *first +*second +*third;
				if (sum % 2) {
					// if sum of indices is odd
					ostringstream o;
					o << *first << *second << *third;
					str_map[ind] = o.str();
				} else {
					str_map[ind];	// blank
				}
			}

	cout << "The next dump should contain half blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	str_map.clean();
	cout << "The next dump should contain no blank entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	*first = *second = *third = 3;
	(*third)--;		// [3][3][2]
	assert(!str_map.erase(ind));
	cout << "Erase non-existent entry, population should remain same.";
	cout << endl;
	cout << "Map's population = " << str_map.population() << endl;

	(*third)++;		// [3][3][3]
	assert(str_map.erase(ind) == 1);
	cout << "The next dump should contain one fewer entry.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	assert(!str_map.erase(ind));
	test_map2d_type::key_type ind2(ind);
	assert(str_map.count(ind2) == 1);	// only [3][3][1] left

	assert(str_map.erase(ind2) == 1);
	assert(!str_map.erase(ind2));
	cout << "The next dump should contain one fewer entry.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	ind2[1] -= 2;
	assert(str_map.count(ind2) == 2);	// only [3][1][1,3] left
	assert(str_map.erase(ind2) == 2);
	assert(str_map.erase(ind2) == 0);
	cout << "The next dump should contain two fewer entries.";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	test_map1d_type::key_type ind1 = 1;
	assert(str_map.count(ind1) == 8);	// only [1][*][*]
	assert(str_map.erase(ind1) == 8);
	assert(str_map.erase(ind1) == 0);
	cout << "The next dump should contain eight fewer entries ([1]).";
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;
	
	{
	// just check for implementation requirements of 1d specialization
	test_map1d_type	map1d;
	map1d[1] = "one";
	assert(!map1d.empty());
	map1d[2];
	assert(!map1d.empty());
	assert(map1d.population() == 2);
	map1d.clean();
	assert(map1d.population() == 1);
	map1d.clear();
	assert(map1d.empty());
	assert(map1d.population() == 0);
	}

	return 0;
}

