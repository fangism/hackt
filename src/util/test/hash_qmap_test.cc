/**
	\file "hash_qmap_test.cc"
	Test for queryable hash map.
	$Id: hash_qmap_test.cc,v 1.8 2006/02/26 02:28:04 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <string>

#include "util/hash_qmap.tcc"
#include "util/memory/excl_ptr.hh"
#include "util/hash_specializations.hh"

#include "util/using_ostream.hh"
using std::string;
using util::hash_qmap;
using namespace util::memory;

typedef	hash_qmap<string, some_ptr<string> > 	test_map_type;

ostream& operator << (ostream& o, const test_map_type& map) {
	test_map_type::const_iterator i = map.begin();
	for ( ; i!=map.end(); i++) {
		some_ptr<const string> s = i->second;
		o << i->first << " -> ";
		if (s)
			o << *s << endl;
		else
			o << "(null)" << endl;
	}
	return o;
}

int main(int, char*[]) {
	test_map_type m;
	cout << m << endl;
	excl_ptr<string> s0(new string("one"));
	assert(s0.owned());
	m["first"] = s0;
	assert(!s0.owned());
	cout << m << endl;
		// this doesn't promise const-ness of the map
//	never_ptr<string> s1 = m["second"];
		// this will promise not to modify the map, just probe
	never_ptr<string> s1 = static_cast<const test_map_type&>(m)["second"];
	assert(!s1);
	cout << m << endl;
	s1 = m["first"];
	assert(s1);
	cout << "s1 = " << *s1 << endl;
	cout << m << endl;

	
#if 0
	// this will not transfer ownership to the map
	some_ptr<string> s2 (excl_ptr<string>(new string("two")));
	assert(s2.owned());
	m["second"] = s2;
	assert(!s2.owned());
	cout << m << endl;
#endif

	return 0;
}

