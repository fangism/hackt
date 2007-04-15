/**
	\file "sparse_collection_test.cc"
	NOTE: this tests a structure in the HAC library, not in util.  
	$Id: sparse_collection_test.cc,v 1.3 2007/04/15 05:52:37 fang Exp $
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
#include "Object/inst/sparse_collection.tcc"

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using HAC::entity::sparse_collection;

/**
	For the sake of disambiguating key_type from value_type.
 */
class mystring : public string {
public:
	mystring(const char* s) : string(s) { }
};

namespace HAC {
namespace entity {

template class sparse_collection<string, mystring>;

}
}

typedef	sparse_collection<string, mystring>	collection_type;

int
main(int, char*[]) {
	collection_type c;
{
	cout << "adding first key-value pair." << endl;
	assert(!c.find("alpha"));
	assert(c.insert("alpha", "beta"));
	assert(c.find("alpha"));
	assert(c.lookup_index(string("alpha")) == 1);
	assert(c.find(1));
	assert(c[1] == "beta");
	assert(c["alpha"] == "beta");
	assert(c.lookup_key(1) == "alpha");
	const mystring& beta(c["alpha"]);
	cout << "beta == " << beta << endl;
	assert(c.lookup_index(beta) == 1);
	assert(c.lookup_key(beta) == "alpha");
	assert(&c[1] == &beta);
	assert(&c["alpha"] == &beta);
}
{
	cout << "checking for failure to insert." << endl;
	assert(!c.insert("alpha", "gamma"));
	assert(!c.find("boo"));
	assert(c.lookup_index(string("boo")) == 0);
}
{
	cout << "adding second key-value pair." << endl;
	assert(!c.find("gamma"));
	assert(c.insert("gamma", "delta"));
	assert(c.find("gamma"));
	assert(c.lookup_index(string("gamma")) == 2);
	assert(c.find(2));
	assert(c.lookup_key(2) == "gamma");
	const mystring& delta(c["gamma"]);
	cout << "delta == " << delta << endl;
	assert(c.lookup_index(delta) == 2);
	assert(c.lookup_key(delta) == "gamma");
}
{
	cout << "adding four more key-value pairs." << endl;
	assert(c.insert("unos", "eins"));
	assert(c.insert("dos", "zwei"));
	assert(c.insert("tres", "drei"));
	assert(c.insert("quatro", "vier"));
	assert(c.size() == 6);
	cout << "c[\"unos\"] = " << c["unos"] << endl;
	cout << "c[\"dos\"] = " << c["dos"] << endl;
	cout << "c[\"tres\"] = " << c["tres"] << endl;
	cout << "c[\"quatro\"] = " << c["quatro"] << endl;
}
	const mystring& eins(c[3]);
	const mystring& zwei(c[4]);
	const mystring& drei(c[5]);
	const mystring& vier(c[6]);
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
	assert(c.lookup_index(string("unos")) == 3);
	assert(c.lookup_index(string("dos")) == 4);
	assert(c.lookup_index(string("tres")) == 5);
	assert(c.lookup_index(string("quatro")) == 6);
	assert(c.lookup_key(3) == "unos");
	assert(c.lookup_key(4) == "dos");
	assert(c.lookup_key(5) == "tres");
	assert(c.lookup_key(6) == "quatro");
	assert(c.lookup_key(eins) == "unos");
	assert(c.lookup_key(zwei) == "dos");
	assert(c.lookup_key(drei) == "tres");
	assert(c.lookup_key(vier) == "quatro");
	// const mystring _vier("vier");	// copy won't work
	// assert(c.lookup_index(_vier) == 0);	// should fail
}
{
	cout << "Testing iterator interface." << endl;
	std::ostream_iterator<mystring> osi(cout, ", ");
	std::copy(c.begin(), c.end(), osi);
	cout << endl;
}
	cout << "End of tests." << endl;
	return 0;
}

