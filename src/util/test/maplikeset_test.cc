/**
	\file "maplikeset_test.cc"
	Testing maplikeset template class adapter.  
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <string>
#include <set>
#include "util/maplikeset.h"

#include "util/using_ostream.h"
using std::string;
using std::set;
using util::maplikeset;
using util::maplikeset_element;
using util::maplikeset_element_derived;


typedef	maplikeset<set<maplikeset_element<int, string> > >
		test_set_type_1;
typedef	maplikeset<set<maplikeset_element_derived<int, string> > >
		test_set_type_2;

// explicit instantiation
template class
maplikeset<set<maplikeset_element<int, string> > >;
template class
maplikeset<set<maplikeset_element_derived<int, string> > >;

template <class E>
ostream&
dump_maplikeset_element(ostream& o, const E& e) {
	return o << "( " << e.get_key() << ", " << e.const_value() << " )";
}

template <class S>
ostream&
dump_maplikeset(ostream& o, const S& m) {
	typedef	typename S::const_iterator	const_iterator;
	const_iterator i = m.begin();
	const const_iterator e = m.end();
	for ( ; i!=e; i++) {
		dump_maplikeset_element(o, *i) << endl;
	}
	return o;
}

template <class S>
static
void
maplikeset_test(void);

//=============================================================================
int
main(int, char*[]) {
	cout << "======= testing maplikeset with non-derived element ======" << endl;
	maplikeset_test<test_set_type_1>();
	cout << "========= testing maplikeset with derived element ========" << endl;
	maplikeset_test<test_set_type_2>();
	return 0;
}

//-----------------------------------------------------------------------------
template <class S>
static
void
maplikeset_test(void) {
	S foo;
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

	foo[0] = foo[1].const_value() +foo[4].const_value();
	cout << "set foo has 1 empty string:" << endl;
	dump_maplikeset(cout, foo) << endl;
}


