/**
	\file "multidimensional_qmap_test.cc"
 	Test for multidimensional, queryable map.
	$Id: multidimensional_qmap_test.cc,v 1.4.16.1 2005/02/17 00:10:24 fang Exp $
 */

#include <iostream>
#include <string>
#include "sstream.h"
using std::ostringstream;

#include "STL/list.tcc"
USING_LIST
using std::string;

#include "using_ostream.h"

// later separate the following into declarations and definitions
#include "multidimensional_qmap.tcc"

using util::multidimensional_qmap;
using util::make_iter_range;

typedef	multidimensional_qmap<3, int, string>	test_map3d_type;

int
main(int argc, char* argv[]) {
	test_map3d_type	str_map;
	str_map.dump(cout << endl << "Map: " << endl);
	cout << "Map's population = " << str_map.population() << endl;

	list<int> ind;
	ind.push_back(0);
	ind.push_back(0);
	ind.push_back(0);

	list<int>::iterator first = ind.begin();
	list<int>::iterator second = first;	second++;
	list<int>::iterator third = second;	third++;

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

	return 0;
}

