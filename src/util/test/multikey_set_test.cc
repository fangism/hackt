/**
	\file "multikey_set_test.cc"
	Test for multikey_set template class.  
	$Id: multikey_set_test.cc,v 1.1.4.1 2005/02/09 04:14:21 fang Exp $
 */

#include <string>
#include <iostream>
#include "multikey_set.tcc"
#include "sstream.h"
#include <set>

#include "using_ostream.h"

using std::string;
using util::multikey;
using util::multikey_set;
using util::multikey_set_element;
using std::ostringstream;

typedef	multikey_set_element<3, int, string>	string_element_3d;
typedef	multikey_set_element<2, int, string>	string_element_2d;
typedef	multikey_set_element<1, int, string>	string_element_1d;

typedef	multikey_set<3, string_element_3d>	string_set_3d_type;
typedef	multikey_set<2, string_element_2d>	string_set_2d_type;
typedef	multikey_set<1, string_element_1d>	string_set_1d_type;

// explicit instantiation
template class multikey_set<3, string_element_3d>;

int
main(int argc, char* argv[]) {
{
	string_set_3d_type a;
	string_set_2d_type b;
	string_set_1d_type c;
}

	string_set_3d_type	str_map;
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	string_set_3d_type::key_type ind;

	// or reference
	int* first = &ind[0];
	int* second = &ind[1];
	int* third = &ind[2];

	str_map.insert(ind, "Hello.");
	str_map.dump(cout << endl << "Set: " << endl);
	{
	// cout << str_map[ind] << endl;
	const string_set_3d_type::const_iterator i = str_map.find(ind);
	assert(i != str_map.end());
	cout << i->get_value() << endl;
	}
	cout << "Set's population = " << str_map.population() << endl;

	{
#if 0
	// already occupied, will not actually replace!
	string_element_3d foo(ind, "Goodbye.");
	str_map.insert(foo);
#else
	str_map[ind] = "Goodbye.";
#endif
	}
	str_map.dump(cout << endl << "Set: " << endl);
	{
	// cout << str_map[ind] << endl;
	const string_set_3d_type::const_iterator i = str_map.find(ind);
	assert(i != str_map.end());
	cout << i->get_value() << endl;
	}
	cout << "Set's population = " << str_map.population() << endl;

	*third = 1;
	{
	const string_set_3d_type::const_iterator i = str_map.find(ind);
	assert(i == str_map.end());		// not found
#if 0
	cout << "Nothing should follow this colon on this line: "
		<< i->get_value() << endl << endl;
#endif
	// create blank
	str_map[ind];
	}

	cout << "The next dump should contain one blank entry.";
	str_map.dump(cout << endl << "Set: " << endl) << endl;
	cout << "Set's population = " << str_map.population() << endl;

	(*third)++;
	static_cast<const string_set_3d_type&>(str_map).find(ind);
				// find prevents blank creation
	cout << "The next dump should still contain one blank entry.";
	str_map.dump(cout << endl << "Set: " << endl) << endl;
	str_map.find(ind);
	cout << "The next dump should still contain one blank entry.";
	str_map.dump(cout << endl << "Set: " << endl) << endl;
	cout << "Set's population = " << str_map.population() << endl;

#if 0
	string foo = str_map[ind];
	cout << "The next dump should still contain one blank entry.";
	str_map.dump(cout << endl << "Set: " << endl) << endl;
	cout << "Set's population = " << str_map.population() << endl;
#endif

	str_map[ind];		// creates a blank
	cout << "The next dump should contain two blank entries.";
	str_map.dump(cout << endl << "Set: " << endl) << endl;
	cout << "Set's population = " << str_map.population() << endl;

	// next create a ton of blanks
	for (*first = 2; *first < 4; (*first)++)
		for (*second = 2; *second < 4; (*second)++)
			for (*third = 2; *third < 4; (*third)++)
				str_map[ind];
	cout << "The next dump should contain many blank entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

#if 0
	// not a true multidimensional array, can't erase subdimensions
	string_set_3d_type::index_arg_type eraser = make_iter_range(ind);
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain same blank entries as before.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	*first = *second = *third = 3;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain one fewer blank entry.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	eraser.second--;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	*second = 2;
	str_map.erase(eraser);
	cout << "The next dump should contain two fewer blank entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	eraser.second--;
	*first = 2;
	cout << "We have " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	str_map.erase(eraser);
	cout << "The next dump should contain four fewer blank entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	eraser.second++;		// can't do ++++;
	eraser.second++;
	cout << "Back to " << distance(eraser.first, eraser.second)+1 <<
		"-dimension eraser." << endl;
	*first = 0; *second = 0; *third = 1;
	str_map.erase(eraser);
	cout << "The next dump should contain one fewer blank entry.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;
#endif

	str_map.clean();
	cout << "The next dump should contain no blank entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

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
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	str_map.clean();
	cout << "The next dump should contain no blank entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	*first = *second = *third = 3;
	(*third)--;		// [3][3][2]
	assert(!str_map.erase(ind));
	cout << "Erase non-existent entry, population should remain same.";
	cout << endl;
	cout << "Set's population = " << str_map.population() << endl;

	(*third)++;		// [3][3][3]
	assert(str_map.erase(ind) == 1);
	cout << "The next dump should contain one fewer entry.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	assert(!str_map.erase(ind));
	string_set_2d_type::key_type ind2(ind);
	assert(str_map.count(ind2) == 1);	// only [3][3][1] left

	cout << "Erasing entry with key-prefix: " << ind2 << endl;
	const size_t erase_count = str_map.erase(ind2);
	cout << "... " << erase_count << " entries removed." << endl;
	assert(erase_count == 1);

	assert(!str_map.erase(ind2));
	cout << "The next dump should contain one fewer entry.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	ind2[1] -= 2;
	assert(str_map.count(ind2) == 2);	// only [3][1][1,3] left
	assert(str_map.erase(ind2) == 2);
	assert(str_map.erase(ind2) == 0);
	cout << "The next dump should contain two fewer entries.";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;

	string_set_1d_type::key_type ind1(1);
	assert(str_map.count(ind1) == 8);	// only [1][*][*]
	assert(str_map.erase(ind1) == 8);
	assert(str_map.erase(ind1) == 0);
	cout << "The next dump should contain eight fewer entries ([1]).";
	str_map.dump(cout << endl << "Set: " << endl);
	cout << "Set's population = " << str_map.population() << endl;
	
	{
	// just check for implementation requirements of 1d specialization
	string_set_1d_type	map1d;
	{
#if 0
	map1d.insert(1, "one");
#elif 1
	map1d.insert(string_set_1d_type::key_type(1), "one");
#else
	string_set_1d_type::value_type foo(1, "one");
	map1d.insert(foo);
#endif
	}
	assert(!map1d.empty());
#if 0
	map1d.insert(2);
#else
	map1d.insert(string_set_1d_type::key_type(2));
#endif
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

