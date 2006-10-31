/**
	\file "collection_pool_test.cc"
	NOTE: this tests a structure in the HAC library, not in util.  
	$Id: collection_pool_test.cc,v 1.1.2.1 2006/10/31 00:28:39 fang Exp $
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

int
main(int argc, char* argv[]) {
	collection_type c;
{
	cout << "adding first value." << endl;
	assert(c.empty());
	assert(c.size() == 0);
	assert(!c.find(1));
	assert(c.push_back("beta"));
	assert(c.size() == 1);
	assert(!c.empty());
	assert(c.find(1));
}
{
	cout << "adding second value." << endl;
	assert(!c.find(2));
	assert(c.push_back("delta"));
	assert(c.size() == 2);
	assert(c.find(2));
}
{
	cout << "adding four more key-value pairs." << endl;
	assert(c.push_back("eins"));
	assert(c.push_back("zwei"));
	assert(c.push_back("drei"));
	assert(c.push_back("vier"));
	assert(c.size() == 6);
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
	std::ostream_iterator<string> osi(cout, ", ");
	std::copy(c.begin(), c.end(), osi);
	cout << endl;
}
	cout << "End of tests." << endl;
	return 0;
}

