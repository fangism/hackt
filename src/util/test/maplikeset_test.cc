/**
	\file "maplikeset_test.cc"
	Testing maplikeset template class adapter.  
 */

#include <iostream>
#include <string>
#include <set>
#include "maplikeset.h"

#include "using_ostream.h"
using std::string;
using std::set;
using util::maplikeset;
using util::maplikeset_element;

typedef	maplikeset<set<maplikeset_element<int, string> > >
		test_set_type;

// explicit instantiation
template class
maplikeset<set<maplikeset_element<int, string> > >;

template <class K, class V>
ostream&
dump_maplikeset_element(ostream& o, const maplikeset_element<K,V>& e) {
	return o << "( " << e.get_key() << ", " << e.value << " )";
}

template <class E>
ostream&
dump_maplikeset(ostream& o, const maplikeset<E>& m) {
	typedef	typename maplikeset<E>::const_iterator	const_iterator;
	const_iterator i = m.begin();
	const const_iterator e = m.end();
	for ( ; i!=e; i++) {
		dump_maplikeset_element(o, *i) << endl;
	}
	return o;
}

//=============================================================================
int
main(int argc, char* argv[]) {
	test_set_type foo;
	cout << "set foo should be empty: " << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[12];
	cout << "set foo should contain exactly 1 empty element: " << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[12];
	cout << "set foo should still contain exactly 1 empty element:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[1] = "one";
	cout << "set foo has one non-empty string:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[1] = "two";
	cout << "set foo has one non-empty string:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[12] = "zero";
	cout << "set foo has no empty strings:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[12] = foo[0];
	cout << "set foo has 2 empty strings:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[4] = " grams of fat";
	cout << "set foo has 2 empty strings:" << endl;
	dump_maplikeset(cout, foo) << endl;

	foo[0] = foo[1].value +foo[4].value;
	cout << "set foo has 1 empty string:" << endl;
	dump_maplikeset(cout, foo) << endl;

	return 0;
}


