/**
	\file "collection_pool_test.cc"
	NOTE: this tests a structure in the HAC library, not in util.  
	$Id: collection_pool_test.cc,v 1.3 2007/04/15 05:52:34 fang Exp $
 */

// always debug with asserts
#ifdef	NDEBUG
#undef	NDEBUG
#endif

#define	ENABLE_STACKTRACE	0

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <cassert>
#include "Object/inst/collection_pool.tcc"

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using HAC::entity::collection_pool;

namespace HAC {
namespace entity {
template class collection_pool<string>;
}
}

typedef	collection_pool<string>	collection_type;
typedef	collection_type::iterator		iterator;
typedef	collection_type::const_iterator		const_iterator;
#if 0
typedef	const_iterator::outer_iterator_type	outer_iterator;
typedef	const_iterator::inner_iterator_type	inner_iterator;
#endif

template <class Iter>
ostream&
dump_nested_iterator(ostream& o, const Iter& i) {
	o << "outer = " << &*i.outer_iterator() <<
		", inner = " << &*i.inner_iterator();
	return o;
}

int
main(int, char*[]) {
	collection_type c;
	std::ostream_iterator<string> osi(cout, ", ");
{
	cout << "adding first value." << endl;
	assert(c.empty());
	assert(c.size() == 0);
	assert(!c.find(1));
	assert(c.begin() == c.end());
	assert(!(c.begin() != c.end()));
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	assert(c.push_back("beta"));
	assert(c.size() == 1);
	assert(!c.empty());
	assert(c.find(1));
	assert(c.begin() != c.end());
	assert(!(c.begin() == c.end()));
#if 0
	dump_nested_iterator(cout << "begin: ", c.begin()) << endl;
	dump_nested_iterator(cout << "end: ", c.end()) << endl;
	dump_nested_iterator(cout << "begin+1: ", ++c.begin()) << endl;
#endif
	assert(++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
}
{
	cout << "adding second value." << endl;
	assert(!c.find(2));
	assert(c.push_back("delta"));
	assert(c.size() == 2);
	assert(c.find(2));
	assert(c.begin() != c.end());
	assert(++++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
}
{
	cout << "adding four more key-value pairs." << endl;
	assert(c.push_back("eins"));
	assert(c.begin() != c.end());
	assert(++++++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	assert(c.push_back("zwei"));
	assert(c.begin() != c.end());
	assert(++++++++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	assert(c.push_back("drei"));
	assert(c.begin() != c.end());
	assert(++++++++++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	assert(c.push_back("vier"));
	assert(c.begin() != c.end());
	assert(++++++++++++c.begin() == c.end());
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	assert(c.size() == 6);
	assert(c.begin() != c.end());
	cout << "c[3] = " << c[3] << endl;
	cout << "c[4] = " << c[4] << endl;
	cout << "c[5] = " << c[5] << endl;
	cout << "c[6] = " << c[6] << endl;
}
	const string& eins(c[3]);
	const string& zwei(c[4]);
	const string& drei(c[5]);
	const string& vier(c[6]);
{
	cout << "testing lookup invariants." << endl;
	assert(c.find(3) == &eins);
	assert(c.find(4) == &zwei);
	assert(c.find(5) == &drei);
	assert(c.find(6) == &vier);
	assert(c.lookup_index(eins) == 3);
	assert(c.lookup_index(zwei) == 4);
	assert(c.lookup_index(drei) == 5);
	assert(c.lookup_index(vier) == 6);
}
{
	cout << "Testing iterator interface." << endl;
	std::copy(c.begin(), c.end(), osi); cout << endl;
	cout << "list = "; std::copy(c.begin(), c.end(), osi); cout << endl;
	// c.dump(cout) << endl;
}
{
	// bulk allocation tests
	collection_type d;
	size_t j = 0;
	for ( ; j<50; ++j) {
		d.allocate(j);
		assert(d.size() == j*(j+1)/2);
	}
}{
	// bulk allocation tests
	collection_type d;
	size_t j = 4;
	for ( ; j<300; j<<=1) {
		d.allocate(j);
	}
}{
	// bulk allocation tests
	size_t j = 0;
	for ( ; j<50; ++j) {
		collection_type d;
		d.allocate(j);
		assert(d.size() == j);
	}
}
	cout << "End of tests." << endl;
	return 0;
}

